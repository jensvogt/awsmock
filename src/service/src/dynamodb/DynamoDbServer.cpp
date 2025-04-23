//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/service/dynamodb/DynamoDbServer.h>

namespace AwsMock::Service {
    DynamoDbServer::DynamoDbServer(Core::PeriodicScheduler &scheduler) : AbstractServer("dynamodb"), _containerService(ContainerService::instance()), _dynamoDbDatabase(Database::DynamoDbDatabase::instance()), _metricService(Monitoring::MetricService::instance()) {

        // Get HTTP configuration values
        const Core::Configuration &configuration = Core::Configuration::instance();
        _workerPeriod = configuration.GetValueInt("awsmock.modules.dynamodb.worker.period");
        _monitoringPeriod = configuration.GetValueInt("awsmock.modules.dynamodb.monitoring.period");
        _containerName = configuration.GetValueString("awsmock.modules.dynamodb.container.name");
        _containerHost = configuration.GetValueString("awsmock.modules.dynamodb.container.host");
        _containerPort = configuration.GetValueInt("awsmock.modules.dynamodb.container.port");
        _imageName = configuration.GetValueString("awsmock.modules.dynamodb.container.image-name");
        _imageTag = configuration.GetValueString("awsmock.modules.dynamodb.container.image-tag");
        _region = configuration.GetValueString("awsmock.region");
        log_debug << "DynamoDB docker endpoint: " << _containerHost << ":" << _containerPort;

        // Check module active
        if (!IsActive("dynamodb")) {
            log_info << "DynamoDb module inactive";
            return;
        }
        log_info << "DynamoDb server starting";

        // Start DynamoDb docker image
        StartLocalDynamoDb();

        // Start DynamoDB monitoring update counters
        scheduler.AddTask("monitoring-dynamodb-counters", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Start synchronizing
        scheduler.AddTask("dynamodb-sync-tables", [this] { this->SynchronizeTables(); }, _workerPeriod);
        scheduler.AddTask("dynamodb-sync-items", [this] { this->SynchronizeItems(); }, _workerPeriod);

        // Set running
        SetRunning();
    }

    void DynamoDbServer::StartLocalDynamoDb() const {
        log_debug << "Starting DynamoDB docker image";

        // Check docker image
        if (!_containerService.ImageExists(_imageName, _imageTag)) {
            const std::string output = _containerService.BuildImage(_imageName, _imageTag, DYNAMODB_DOCKER_FILE);
            log_trace << "Image " << _imageName << " output: " << output;
        }

        // Check container image
        if (!_containerService.ContainerExistsByImageName(_imageName, _imageTag)) {
            const Dto::Docker::CreateContainerResponse response = _containerService.CreateContainer(_imageName, _imageTag, _containerName, _containerPort, _containerPort);
            _containerService.StartDockerContainer(response.id);
            _containerService.WaitForContainer(response.id);
            log_trace << "CreateContainer, containerName: " << _containerName << " id: " << response.id;
        }

        // Start docker container, in case it is not already running.
        if (const Dto::Docker::Container container = _containerService.GetContainerByName(_containerName); container.state != "running") {
            _containerService.StartDockerContainer(container.id);
            _containerService.WaitForContainer(container.id);
            log_info << "Docker containers for DynamoDB started";
        } else {
            log_info << "Docker containers for DynamoDB already running";
        }
    }

    void DynamoDbServer::StopLocalDynamoDb() const {
        log_debug << "Starting DynamoDB docker image";

        // Check docker image
        if (!_containerService.ImageExists(_imageName, _imageTag)) {
            log_error << "Image " << _imageName << " does not exist";
            throw Core::ServiceException("Image does not exist");
        }

        // Check container image
        if (!_containerService.ContainerExistsByName(_containerName)) {
            log_error << "Container " << _imageName << " does not exist";
            throw Core::ServiceException("Container does not exist");
        }

        // Stop docker container, in case it is running.
        if (const Dto::Docker::Container container = _containerService.GetFirstContainerByImageName(_imageName, _imageTag);
            container.state == "running") {
            _containerService.StopContainer(container);
            log_info << "Docker containers for DynamoDB stopped";
        } else {
            log_info << "Docker containers for DynamoDB not running";
        }
    }

    void DynamoDbServer::SynchronizeTables() const {

        try {

            // Get the list of tables from DynamoDB
            Dto::DynamoDb::ListTableRequest request;
            request.region = Core::Configuration::instance().GetValueString("awsmock.region");
            request.PrepareRequest();
            if (const Dto::DynamoDb::ListTableResponse listTableResponse = _dynamoDbService.ListTables(request); !listTableResponse.tableNames.empty()) {

                for (const auto &tableName: listTableResponse.tableNames) {

                    Dto::DynamoDb::DescribeTableRequest describeTableRequest;
                    describeTableRequest.region = _region;
                    describeTableRequest.tableName = tableName;
                    describeTableRequest.PrepareRequest();
                    Dto::DynamoDb::DescribeTableResponse describeTableResponse = _dynamoDbService.DescribeTable(describeTableRequest);
                    describeTableResponse.region = _region;
                    describeTableResponse.PrepareResponse();

                    Database::Entity::DynamoDb::Table table = Dto::DynamoDb::Mapper::map(describeTableResponse);
                    table = _dynamoDbDatabase.CreateOrUpdateTable(table);
                    log_debug << "Table synchronized, table: " << table.name;
                }

            } else {

                const long count = _dynamoDbDatabase.DeleteAllTables();
                log_debug << "Tables deleted, count: " << count;
            }
            log_debug << "DynamoDB tables synchronized";

        } catch (Core::JsonException &exc) {
            log_error << exc.message();
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
        } catch (std::exception &exc) {
            log_error << exc.what();
        }
    }

    void DynamoDbServer::SynchronizeItems() const {

        try {

            // Get the list of tables from DynamoDB
            Dto::DynamoDb::ListTableRequest request;
            request.region = Core::Configuration::instance().GetValueString("awsmock.region");
            request.PrepareRequest();
            if (const Dto::DynamoDb::ListTableResponse listTableResponse = _dynamoDbService.ListTables(request); !listTableResponse.tableNames.empty()) {

                for (const auto &tableName: listTableResponse.tableNames) {

                    Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(_region, tableName);

                    long size = 0;
                    Dto::DynamoDb::ScanRequest scanRequest;
                    scanRequest.region = _region;
                    scanRequest.tableName = tableName;
                    scanRequest.PrepareRequest();
                    Dto::DynamoDb::ScanResponse scanResponse = _dynamoDbService.Scan(scanRequest);
                    scanResponse.region = _region;
                    scanResponse.PrepareResponse(table);

                    if (!scanResponse.items.empty()) {
                        for (auto &item: scanResponse.items) {
                            item = _dynamoDbDatabase.CreateOrUpdateItem(item);
                            log_trace << "Item synchronized, item: " << item.oid;
                            size += item.ToJson().size();
                        }
                    }

                    // Adjust table counters
                    table.itemCount = scanResponse.count;
                    table.size = size;
                    table = _dynamoDbDatabase.UpdateTable(table);
                    log_debug << "Table counter adjusted, table: " << table.name;
                }
            }
            log_debug << "DynamoDB items synchronized";

        } catch (Core::JsonException &exc) {
            log_error << exc.message();
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
        } catch (std::exception &exc) {
            log_error << exc.what();
        }
    }

    void DynamoDbServer::UpdateCounter() const {
        log_trace << "Dynamodb monitoring starting";

        const long tables = _dynamoDbDatabase.CountTables();
        const long items = _dynamoDbDatabase.CountItems();
        _metricService.SetGauge(DYNAMODB_TABLE_COUNT, static_cast<double>(tables));
        _metricService.SetGauge(DYNAMODB_ITEM_COUNT, static_cast<double>(items));

        log_trace << "DynamoDb monitoring finished";
    }
}// namespace AwsMock::Service
