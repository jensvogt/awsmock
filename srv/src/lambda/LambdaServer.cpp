//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/lambda/LambdaServer.h>

namespace AwsMock::Service {
    LambdaServer::LambdaServer(Core::Scheduler &scheduler, boost::asio::io_context &ioc) : AbstractServer("lambda"), _lambdaDatabase(Database::LambdaDatabase::instance()), _lambdaService(ioc),
                                                                                           _monitoringCollector(Core::MonitoringCollector::instance()), _scheduler(scheduler) {

        const Core::Configuration &configuration = Core::Configuration::instance();
        _region = configuration.GetValue<std::string>("awsmock.region");
        _lifetime = configuration.GetValue<int>("awsmock.modules.lambda.lifetime");
        _counterPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.lambda.counter-period");
        _logRetentionPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.lambda.log-retention-period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.lambda.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.backup.cron");
        log_debug << "Lambda remove period: " << _lifetime << ", counterPeriod: " << _counterPeriod << ", logRetentionPeriod: " << _logRetentionPeriod;

        // Directories
        _lambdaDir = configuration.GetValue<std::string>("awsmock.modules.lambda.data-dir");
        Core::DirUtils::EnsureDirectory(_lambdaDir);
        log_debug << "Lambda directory: " << _lambdaDir;

        // Cleanup instances
        CleanupInstances();

        // Cleanup container
        CleanupDocker();

        // Create a local network if it does not exist yet
        CreateLocalNetwork();

        // Start the lambdas, this will build the containers, if not already existing
        CreateContainers();

        // Start lambda monitoring update counters
        _scheduler.AddTask("lambda-monitoring", [this] { UpdateCounter(); }, _counterPeriod);
        log_debug << "Lambda task started, name monitoring-lambda-counters, period: " << _counterPeriod;

        // Start the delete old lambda task
        _scheduler.AddTask("lambda-remove", [this] { RemoveExpiredLambdas(); }, _lifetime);
        log_debug << "Lambda task started, name lambda-remove-lambdas, period: " << _lifetime;

        // Start the delete old lambda logs
        _scheduler.AddTask("lambda-remove-logs", [this] { RemoveExpiredLambdaLogs(); }, _logRetentionPeriod * 24 * 60 * 60);
        log_debug << "Lambda task started, name remove-lambda-logs, period: " << _logRetentionPeriod;

        // Start backup
        if (_backupActive) {
            _scheduler.AddTask("lambda-backup", [] { BackupLambda(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&LambdaServer::Shutdown, this));

        log_debug << "Lambda server initialized";
    }

    void LambdaServer::Shutdown() {
        log_debug << "Lambda server shutdown, region: " << _region;

        _scheduler.Shutdown("lambda-monitoring");
        _scheduler.Shutdown("lambda-remove");
        _scheduler.Shutdown("lambda-remove-logs");
        _scheduler.Shutdown("lambda-backup");

        // Stop all lambda docker containers
        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(_region); auto &lambda: lambdas) {

            // Cleanup instances
            for (const auto &instance: lambda.instances) {
                ContainerService::instance().KillContainer(instance.containerId);
                log_debug << "Lambda instances cleaned up, id: " << instance.containerId;
            }
            lambda.instances.clear();
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_info << "Lambda stopped, name: " << lambda.function;
        }
        log_info << "Lambda server stopped";
    }

    void LambdaServer::CleanupDocker() const {
        _dockerService.PruneContainers();
        log_debug << "Docker containers cleaned up";
    }

    void LambdaServer::CleanupInstances() const {
        log_debug << "Cleanup lambdas";

        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(_region); auto &lambda: lambdas) {
            log_debug << "Get containers";
            for (std::vector<Dto::Docker::Container> containers = _dockerService.ListContainerByImageName(lambda.function, lambda.dockerTag); const auto &container: containers) {
                ContainerService::instance().KillContainer(container.id);
                ContainerService::instance().DeleteContainer(container.id);
            }
            lambda.instances.clear();
            lambda.state = Database::Entity::Lambda::Inactive;
            lambda = _lambdaDatabase.UpdateLambda(lambda);
        }
        log_debug << "Lambda instances cleaned up";
    }

    void LambdaServer::CreateLocalNetwork() const {
        log_debug << "Create networks, name: local";

        if (!_dockerService.NetworkExists("local")) {
            Dto::Docker::CreateNetworkRequest request;
            request.name = "local";
            request.driver = "bridge";

            Dto::Docker::CreateNetworkResponse response = _dockerService.CreateNetwork(request);
            log_debug << "Docker network created, name: " << request.name << " driver: " << request.driver << " id: " << response.id;
        } else {
            log_debug << "Docker network exists already, name: local";
        }
    }

    void LambdaServer::RemoveExpiredLambdas() const {

        // Get the list of lambdas
        Database::Entity::Lambda::LambdaList lambdaList = _lambdaDatabase.ListLambdas();
        if (lambdaList.empty()) {
            return;
        }
        log_debug << "Lambda lifetime starting, count: " << lambdaList.size();

        // Get lifetime from configuration
        const auto expired = system_clock::now() - std::chrono::seconds(_lifetime);

        // Loop over lambdas and remove expired instances
        for (auto &lambda: lambdaList) {

            if (lambda.instances.empty()) {
                continue;
            }

            // Remove instance
            const auto count = std::erase_if(lambda.instances, [lambda, expired](const Database::Entity::Lambda::Instance &instance) {
                if (instance.created > system_clock::time_point::min() && instance.created < expired) {
                    ContainerService::instance().StopContainer(instance.containerId);
                    log_info << "Lambda instance stopped, lambda: " << lambda.function << ", containerId: " << instance.containerId;
                    return true;
                }
                return false;
            });

            // Update lambda
            if (count > 0) {
                lambda = _lambdaDatabase.UpdateLambda(lambda);
                log_debug << "Lambda updated, function" << lambda.function << " removed: " << count;
                _dockerService.PruneContainers();
            }
        }
        log_debug << "Lambda worker finished, count: " << lambdaList.size();
    }

    void LambdaServer::RemoveExpiredLambdaLogs() const {

        // Cleanup logs
        const system_clock::time_point cutOff = system_clock::now() - std::chrono::days(_logRetentionPeriod);
        const long count = _lambdaDatabase.RemoveExpiredLambdaLogs(cutOff);
        log_debug << "Lambda logs removed, cutOff: " << Core::DateTimeUtils::ToISO8601(cutOff) << ", count: " << count;
    }

    void LambdaServer::UpdateCounter() const {
        log_trace << "Lambda monitoring starting";

        // Get the lambda list
        const Database::Entity::Lambda::LambdaList lambdas = _lambdaDatabase.ListLambdas();
        _monitoringCollector.SetGauge(LAMBDA_FUNCTION_COUNT, static_cast<double>(lambdas.size()));

        if (lambdas.empty()) {
            return;
        }

        for (const auto &lambda: lambdas) {
            _monitoringCollector.SetGauge(LAMBDA_INSTANCES_COUNT, "function_name", lambda.function, static_cast<double>(lambda.instances.size()));
        }
        log_trace << "Lambda monitoring finished";
    }

    void LambdaServer::CreateContainers() const {

        try {

            // Loop over lambdas and create the containers
            for (Database::Entity::Lambda::LambdaList lambdas = _lambdaDatabase.ListLambdas(_region); auto &lambda: lambdas) {

                if (lambda.enabled) {
                    log_info << "Starting lambda container, function: " << lambda.function;
                    Dto::Lambda::StartLambdaRequest request;
                    request.region = _region;
                    request.functionArn = lambda.arn;
                    _lambdaService.StartLambda(request);
                    log_info << "Finished starting lambda container, function: " << lambda.function;
                } else {
                    lambda.state = Database::Entity::Lambda::Inactive;
                    _lambdaDatabase.UpdateLambda(lambda);
                }
            }
            log_debug << "Lambda containers started";

        } catch (Core::ServiceException &e) {
            log_error << e.message();
        }
    }

    void LambdaServer::BackupLambda() {
        ModuleService::BackupModule("lambda");
    }

}// namespace AwsMock::Service
