//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/service/dynamodb/DynamoDbServer.h>

namespace AwsMock::Service {

    DynamoDbServer::DynamoDbServer(Core::Scheduler &scheduler) : AbstractServer("dynamodb"), _containerService(ContainerService::instance()), _dynamoDbDatabase(Database::DynamoDbDatabase::instance()), _metricService(Monitoring::MetricService::instance()) {

        // Get HTTP configuration values
        const Core::Configuration &configuration = Core::Configuration::instance();
        _backupActive = configuration.GetValue<bool>("awsmock.modules.dynamodb.backup.active");
        _backupCron = configuration.GetValue<std::string>("awsmock.modules.dynamodb.backup.cron");
        _workerPeriod = configuration.GetValue<int>("awsmock.modules.dynamodb.worker.period");
        _monitoringPeriod = configuration.GetValue<int>("awsmock.modules.dynamodb.monitoring.period");
        _containerName = configuration.GetValue<std::string>("awsmock.modules.dynamodb.container.name");
        _containerHost = configuration.GetValue<std::string>("awsmock.modules.dynamodb.container.host");
        _containerPort = configuration.GetValue<int>("awsmock.modules.dynamodb.container.port");
        _imageName = configuration.GetValue<std::string>("awsmock.modules.dynamodb.container.image-name");
        _imageTag = configuration.GetValue<std::string>("awsmock.modules.dynamodb.container.image-tag");
        _region = configuration.GetValue<std::string>("awsmock.region");
        _dataDir = configuration.GetValue<std::string>("awsmock.modules.dynamodb.data-dir");
        log_debug << "DynamoDB docker endpoint: " << _containerHost << ":" << _containerPort;

        // Check module active
        if (!IsActive("dynamodb")) {
            log_info << "DynamoDb module inactive";
            return;
        }
        log_info << "DynamoDb server starting";

        // Create a local network if it is not existing yet
        CreateLocalNetwork();

        // Start DynamoDb docker image
        StartLocalDynamoDb();

        // Start DynamoDB monitoring update counters
        scheduler.AddTask("dynamodb-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Start synchronizing
        scheduler.AddTask("dynamodb-sync-tables", [this] { this->SynchronizeTables(); }, _workerPeriod, _workerPeriod);
        scheduler.AddTask("dynamodb-sync-items", [this] { this->SynchronizeItems(); }, _workerPeriod, _workerPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("dynamodb-backup", [this] { BackupDynamoDb(); }, _backupCron);
        }

        // Set running
        SetRunning();
    }

    void DynamoDbServer::CreateLocalNetwork() const {
        log_debug << "Create networks, name: local";

        if (!_containerService.NetworkExists("local")) {
            Dto::Docker::CreateNetworkRequest request;
            request.name = "local";
            request.driver = "bridge";

            Dto::Docker::CreateNetworkResponse response = _containerService.CreateNetwork(request);
            log_debug << "Docker network created, name: " << request.name << " driver: " << request.driver << " id: " << response.id;
        } else {
            log_debug << "Docker network exists already, name: local";
        }
    }

    void DynamoDbServer::StartLocalDynamoDb() const {
        log_debug << "Starting DynamoDB docker image";

        // Check docker image
        if (!_containerService.ImageExists(_imageName, _imageTag)) {
            const std::string dockerString = WriteDockerFile();
            const std::string output = _containerService.BuildDynamoDbImage(_imageName, _imageTag, dockerString);
            log_trace << "Image " << _imageName << " output: " << output;
        }

        // Check container image
        if (!_containerService.ContainerExistsByImageName(_imageName, _imageTag)) {
            const Dto::Docker::CreateContainerResponse response = _containerService.CreateContainer(_imageName, _imageTag, _containerName, _containerPort, _containerPort);
            log_info << "Docker container created, name: " << _containerName << " id: " << response.id;
        }

        // Start the docker container, in case it is not already running.
        if (const Dto::Docker::Container container = _containerService.GetContainerByName(_containerName); !container.state.running) {
            _containerService.StartDockerContainer(container.id, _containerName);
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
        if (!_containerService.ContainerExists(_containerName)) {
            log_error << "Container " << _imageName << " does not exist";
            throw Core::ServiceException("Container does not exist");
        }

        // Stop the docker container, in case it is running.
        if (const Dto::Docker::Container container = _containerService.GetFirstContainerByImageName(_imageName, _imageTag); container.state.running) {
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
            request.region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
            if (const Dto::DynamoDb::ListTableResponse listTableResponse = _dynamoDbService.ListTables(request); !listTableResponse.tableNames.empty()) {

                for (const auto &tableName: listTableResponse.tableNames) {

                    Dto::DynamoDb::DescribeTableRequest describeTableRequest;
                    describeTableRequest.region = _region;
                    describeTableRequest.tableName = tableName;
                    Dto::DynamoDb::DescribeTableResponse describeTableResponse = _dynamoDbService.DescribeTable(describeTableRequest);
                    describeTableResponse.region = _region;

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
            request.region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
            if (const Dto::DynamoDb::ListTableResponse listTableResponse = _dynamoDbService.ListTables(request); !listTableResponse.tableNames.empty()) {

                for (const auto &tableName: listTableResponse.tableNames) {

                    Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(_region, tableName);

                    const long size = 0;
                    Dto::DynamoDb::ScanRequest scanRequest;
                    scanRequest.region = _region;
                    scanRequest.tableName = tableName;
                    Dto::DynamoDb::ScanResponse scanResponse = _dynamoDbService.Scan(scanRequest);
                    scanResponse.region = _region;

                    if (!scanResponse.items.empty()) {
                        for (auto &item: scanResponse.items) {
                            /*Database::Entity::DynamoDb::Item itemEntity = Dto::DynamoDb::Mapper::map(item);
                            itemEntity = _dynamoDbDatabase.CreateOrUpdateItem(itemEntity);
                            log_trace << "Item synchronized, item: " << itemEntity.oid;
                            size += item.size();*/
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

    std::string DynamoDbServer::WriteDockerFile() {
        std::stringstream ss;
        ss << "FROM amazon/dynamodb-local:latest" << std::endl;
        ss << "VOLUME /usr/local/awsmock/data/dynamodb /home/dynamodblocal/data" << std::endl;
        ss << "WORKDIR /home/dynamodblocal" << std::endl;
        ss << "EXPOSE 8000 8000" << std::endl;
        ss << R"(ENTRYPOINT ["java", "-Djava.library.path=./DynamoDBLocal_lib", "-jar", "DynamoDBLocal.jar", "-sharedDb"])";
        return ss.str();
    }

    void DynamoDbServer::UpdateCounter() const {
        log_trace << "Dynamodb monitoring starting";

        std::vector<Database::Entity::DynamoDb::Table> tables = _dynamoDbDatabase.ListTables();
        if (tables.empty()) {
            return;
        }

        long totalItems = 0;
        long totalSize = 0;
        for (auto const &table: tables) {

            totalItems += table.itemCount;
            totalSize += table.size;

            _metricService.SetGauge(DYNAMODB_ITEMS_BY_TABLE, "table", table.name, table.itemCount);
            _metricService.SetGauge(DYNAMODB_SIZE_BY_TABLE, "table", table.name, table.size);
        }
        _metricService.SetGauge(DYNAMODB_TABLE_COUNT, {}, {}, static_cast<double>(tables.size()));
        _metricService.SetGauge(DYNAMODB_ITEM_COUNT, {}, {}, totalItems);
        _metricService.SetGauge(DYNAMODB_TABLE_SIZE, {}, {}, totalSize);

        log_trace << "DynamoDb monitoring finished";
    }

    void DynamoDbServer::BackupDynamoDb() {
        ModuleService::BackupModule("dynamodb", true);
    }

    void DynamoDbServer::Shutdown() {
        log_debug << "DynamoDb server shutdown, region: " << _region;

        // Stop detached instances
        /*for (const auto &instance: ContainerService::instance().ListContainerByImageName(_imageName, _imageTag)) {
            ContainerService::instance().StopContainer(instance.id);
            ContainerService::instance().DeleteContainer(instance.id);
            log_debug << "Detached dynamodb instances cleaned up, id: " << instance.id;
        }*/
        log_info << "All dynamodb instances stopped";
    }

}// namespace AwsMock::Service
