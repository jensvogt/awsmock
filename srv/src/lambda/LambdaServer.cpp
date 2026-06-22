//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/lambda/LambdaServer.h>

namespace Awsmock::Service {

    LambdaServer::LambdaServer() : AbstractServer("lambda") {

        const Core::Configuration &configuration = Core::Configuration::instance();
        _region = configuration.get<std::string>("awsmock.region");
        _lifetime = configuration.get<int>("awsmock.modules.lambda.lifetime");
        _removePeriod = configuration.get<int>("awsmock.modules.lambda.remove-period");
        _counterPeriod = Core::Configuration::instance().get<int>("awsmock.modules.lambda.counter-period");
        _logRetentionPeriod = Core::Configuration::instance().get<int>("awsmock.modules.lambda.log-retention-period");
        _backupActive = Core::Configuration::instance().get<bool>("awsmock.modules.lambda.backup.active");
        _backupCron = Core::Configuration::instance().get<std::string>("awsmock.modules.lambda.backup.cron");
        _lambdaDir = configuration.get<std::string>("awsmock.modules.lambda.data-dir");
        log_debug << "Lambda lifetime period: " << _lifetime << ", counterPeriod: " << _counterPeriod << ", logRetentionPeriod: " << _logRetentionPeriod;

        // Startup task
        Core::Scheduler::instance().AddTask("lambda-initialization", [this] { Initialize(); });
        log_debug << "Lambda initialization started, name: initialization";

        // Start lambda monitoring update counters
        Core::Scheduler::instance().AddTask("lambda-monitoring", [this] { UpdateCounter(); }, _counterPeriod, _counterPeriod);
        log_debug << "Lambda task started, name monitoring-lambda-counters, period: " << _counterPeriod;

        // Start the delete old lambda task
        Core::Scheduler::instance().AddTask("lambda-remove", [this] { RemoveExpiredLambdas(); }, _removePeriod, _removePeriod);
        log_debug << "Lambda task started, name lambda-remove-lambdas, period: " << _removePeriod;

        // Start delete old lambda logs
        const long interval = _logRetentionPeriod * 24 * 60 * 60;
        Core::Scheduler::instance().AddTask("lambda-remove-logs", [this] { RemoveExpiredLambdaLogs(); }, interval, interval);
        log_debug << "Lambda task started, name remove-lambda-logs, period: " << _logRetentionPeriod;

        // Start backup
        if (_backupActive) {
            Core::Scheduler::instance().AddTask("lambda-backup", [] { BackupLambda(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&LambdaServer::shutdown, this));

        log_debug << "Lambda server initialized";
    }

    void LambdaServer::shutdown() {
        log_debug << "Lambda server shutdown, region: " << _region;

        Core::EventBus::instance().sigLambdaStopAll(_region);

        _lambdaController.shutdown();
        Core::Scheduler::instance().Shutdown("lambda-monitoring");
        Core::Scheduler::instance().Shutdown("lambda-remove");
        Core::Scheduler::instance().Shutdown("lambda-remove-logs");
        Core::Scheduler::instance().Shutdown("lambda-backup");

        log_info << "Lambda server stopped";
    }

    void LambdaServer::Initialize() {

        // Directories
        Core::DirUtils::EnsureDirectoryExists(_lambdaDir);
        log_debug << "Lambda directory: " << _lambdaDir;

        // Cleanup instances
        CleanupInstances();

        // Cleanup container
        //CleanupDocker();

        // Create a local network if it does not exist yet
        CreateLocalNetwork();

        // Start the lambdas, this will build the containers, if not already existing
        CreateContainers();
    }

    void LambdaServer::CleanupDocker() const {
        //_dockerService.PruneContainers();
        //log_debug << "Docker containers cleaned up";
    }

    void LambdaServer::CleanupInstances() const {
        log_debug << "Cleanup lambdas";

        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase->listLambdas(_region); auto &lambda: lambdas) {
            log_debug << "Get containers";
            for (std::vector<Dto::Docker::Container> containers = _dockerService.ListContainerByImageName(lambda.function, lambda.dockerTag); const auto &container: containers) {
                ContainerService::instance().KillContainer(container.id);
                ContainerService::instance().DeleteContainer(container.id);
            }
            lambda.instances.clear();
            lambda = _lambdaDatabase->updateLambda(lambda);
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
        const Database::Entity::Lambda::LambdaList lambdaList = _lambdaDatabase->listLambdas({});
        if (lambdaList.empty()) {
            return;
        }
        log_debug << "Lambda lifetime starting, count: " << lambdaList.size();

        // Get expiration time
        const auto expired = system_clock::now() - std::chrono::seconds(_lifetime);

        // Fire stop signal for any function that has at least one expired instance
        for (const auto &lambda: lambdaList) {

            if (lambda.instances.empty()) {
                continue;
            }

            const bool hasExpired = std::ranges::any_of(lambda.instances,
                                                        [expired](const Database::Entity::Lambda::Instance &instance) {
                                                            return instance.created > system_clock::time_point::min() && instance.created < expired;
                                                        });

            if (hasExpired) {
                log_info << "Lambda instances expired, stopping function: " << lambda.function;
                Core::EventBus::instance().sigLambdaStop(lambda.arn, _region);
            }
        }
        log_debug << "Lambda worker finished, count: " << lambdaList.size();
    }

    void LambdaServer::RemoveExpiredLambdaLogs() const {

        // Cleanup logs
        const system_clock::time_point cutOff = system_clock::now() - std::chrono::days(_logRetentionPeriod);
        const long count = _lambdaDatabase->removeExpiredLambdaLogs(cutOff);
        log_debug << "Lambda logs removed, cutOff: " << Core::DateTimeUtils::ToISO8601(cutOff) << ", count: " << count;
    }

    void LambdaServer::UpdateCounter() const {
        log_trace << "Lambda monitoring starting";

        // Get the lambda list
        const Database::Entity::Lambda::LambdaList lambdas = _lambdaDatabase->listLambdas({});
        Core::EventBus::instance().sigMetricGauge(LAMBDA_FUNCTION_COUNT, {}, {}, static_cast<double>(lambdas.size()));

        if (lambdas.empty()) {
            return;
        }

        for (const auto &lambda: lambdas) {
            Core::EventBus::instance().sigMetricGauge(LAMBDA_INSTANCES_COUNT, "function_name", lambda.function, static_cast<double>(lambdas.size()));
        }
        log_trace << "Lambda monitoring finished";
    }

    void LambdaServer::CreateContainers() const {
        Core::EventBus::instance().sigLambdaStartAll(_region);
        log_debug << "Lambda containers started";
    }

    void LambdaServer::BackupLambda() {
        ModuleService{}.BackupModule("lambda", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

}// namespace Awsmock::Service
