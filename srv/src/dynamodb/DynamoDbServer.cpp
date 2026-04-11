//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/service/dynamodb/DynamoDbServer.h>

namespace AwsMock::Service {

    DynamoDbServer::DynamoDbServer(Core::Scheduler &scheduler) : AbstractServer("dynamodb"), _containerService(ContainerService::instance()), _dynamoDbDatabase(Database::DynamoDbDatabase::instance()), _scheduler(scheduler) {

        // Get HTTP configuration values
        const Core::Configuration &configuration = Core::Configuration::instance();
        _backupActive = configuration.GetValue<bool>("awsmock.modules.dynamodb.backup.active");
        _backupCron = configuration.GetValue<std::string>("awsmock.modules.dynamodb.backup.cron");
        _workerPeriod = configuration.GetValue<int>("awsmock.modules.dynamodb.worker-period");
        _monitoringPeriod = configuration.GetValue<int>("awsmock.modules.dynamodb.monitoring-period");
        _region = configuration.GetValue<std::string>("awsmock.region");
        _dataDir = configuration.GetValue<std::string>("awsmock.modules.dynamodb.data-dir");

        // Create a local network if it is not existing yet
        CreateLocalNetwork();

        // Start DynamoDB monitoring update counters
        _scheduler.AddTask("dynamodb-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            _scheduler.AddTask("dynamodb-backup", [] { BackupDynamoDb(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&DynamoDbServer::Shutdown, this));

        log_info << "DynamoDB server started";
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

    void DynamoDbServer::UpdateCounter() const {
        log_trace << "Dynamodb monitoring starting";

        const std::vector<Database::Entity::DynamoDb::Table> tables = _dynamoDbDatabase.ListTables();
        if (tables.empty()) {
            return;
        }

        // Reload the counters first
        _dynamoDbDatabase.AdjustItemCounters();

        long totalItems = 0;
        long totalSize = 0;
        for (auto const &table: tables) {

            totalItems += table.items;
            totalSize += table.size;

            Core::EventBus::instance().sigMetricGauge(DYNAMODB_ITEMS_BY_TABLE, "table", table.name, table.items);
            Core::EventBus::instance().sigMetricGauge(DYNAMODB_SIZE_BY_TABLE, "table", table.name, table.size);
        }
        Core::EventBus::instance().sigMetricGauge(DYNAMODB_TABLE_COUNT, {}, {}, static_cast<double>(tables.size()));
        Core::EventBus::instance().sigMetricGauge(DYNAMODB_ITEM_COUNT, {}, {}, totalItems);
        Core::EventBus::instance().sigMetricGauge(DYNAMODB_TABLE_SIZE, {}, {}, totalSize);

        log_trace << "DynamoDb monitoring finished";
    }

    void DynamoDbServer::BackupDynamoDb() {
        ModuleService::BackupModule("dynamodb", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void DynamoDbServer::Shutdown() {
        log_debug << "DynamoDb server shutdown, region: " << _region;
        _scheduler.Shutdown("dynamodb-monitoring");
        _scheduler.Shutdown("dynamodb-backup");
        log_info << "DynamoDB server stopped";
    }

} // namespace AwsMock::Service
