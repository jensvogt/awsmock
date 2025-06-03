//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/lambda/LambdaServer.h>

namespace AwsMock::Service {
    LambdaServer::LambdaServer(Core::PeriodicScheduler &scheduler) : AbstractServer("lambda"), _lambdaDatabase(Database::LambdaDatabase::instance()) {

        const Core::Configuration &configuration = Core::Configuration::instance();
        _counterPeriod = configuration.GetValue<int>("awsmock.modules.lambda.counter-period");
        _lifetime = configuration.GetValue<int>("awsmock.modules.lambda.lifetime");
        _logRetentionPeriod = configuration.GetValue<int>("awsmock.modules.lambda.log-retention-period");
        log_debug << "Lambda remove period: " << _lifetime << ", counterPeriod: " << _counterPeriod << ", logRetentionPeriod: " << _logRetentionPeriod;

        // Directories
        _lambdaDir = configuration.GetValue<std::string>("awsmock.modules.lambda.data-dir");
        log_debug << "Lambda directory: " << _lambdaDir;

        // Create environment
        _region = configuration.GetValue<std::string>("awsmock.region");

        // Initialize shared memory
        _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        _lambdaCounterMap = _segment.find<Database::LambdaCounterMapType>(Database::LAMBDA_COUNTER_MAP_NAME).first;

        // Create lambda directory
        Core::DirUtils::EnsureDirectory(_lambdaDir);

        // Cleanup instances
        CleanupInstances();

        // Cleanup container
        CleanupDocker();

        // Create a local network if it does not exist yet
        CreateLocalNetwork();

        // Start the lambdas, this will build the containers, if not already existing
        CreateContainers();

        // Start lambda monitoring update counters
        scheduler.AddTask("monitoring-lambda-counters", [this] { UpdateCounter(); }, _counterPeriod);
        log_debug << "Lambda task started, name monitoring-lambda-counters, period: " << _counterPeriod;

        // Start the delete old lambda task
        scheduler.AddTask("remove-lambdas", [this] { RemoveExpiredLambdas(); }, _lifetime);
        log_debug << "Lambda task started, name lambda-remove-lambdas, period: " << _lifetime;

        // Start the delete old lambda logs
        scheduler.AddTask("remove-lambda-logs", [this] { RemoveExpiredLambdaLogs(); }, _logRetentionPeriod * 24 * 60 * 60);
        log_debug << "Lambda task started, name remove-lambda-logs, period: " << _logRetentionPeriod;

        // Set running
        SetRunning();
        log_debug << "Lambda server initialized";
    }

    void LambdaServer::Shutdown() {
        log_debug << "Lambda server shutdown, region: " << _region;

        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(_region); auto &lambda: lambdas) {

            // Cleanup instances
            for (const auto &instance: lambda.instances) {
                ContainerService::instance().StopContainer(instance.containerId);
                ContainerService::instance().DeleteContainer(instance.containerId);
                log_debug << "Lambda instances cleaned up, id: " << instance.containerId;
            }
            lambda.instances.clear();
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_debug << "Lambda cleaned up, name: " << lambda.function;
        }
        log_info << "All lambda instances stopped";
    }

    void LambdaServer::CleanupDocker() const {
        _dockerService.PruneContainers();
        log_debug << "Docker containers cleaned up";
    }

    void LambdaServer::CleanupInstances() const {
        log_debug << "Cleanup lambdas";

        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(_region); auto &lambda: lambdas) {
            log_debug << "Get containers";
            for (std::vector<Dto::Docker::Container> containers = _dockerService.ListContainerByImageName(lambda.function, "latest"); const auto &container: containers) {
                ContainerService::instance().StopContainer(container.id);
                ContainerService::instance().DeleteContainer(container.id);
            }
            lambda.instances.clear();
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

            auto [id, warning] = _dockerService.CreateNetwork(request);
            log_debug << "Docker network created, name: " << request.name << " driver: " << request.driver << " id: " << id;
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
        if (lambdas.empty()) {
            return;
        }
        _metricService.SetGauge(LAMBDA_FUNCTION_COUNT, {}, {}, static_cast<double>(lambdas.size()));

        for (const auto &lambda: lambdas) {
            double averageRuntime = 0.0;
            if ((*_lambdaCounterMap)[lambda.arn].invocations > 0) {
                averageRuntime = (*_lambdaCounterMap)[lambda.arn].averageRuntime / (double) (*_lambdaCounterMap)[lambda.arn].invocations;
            }
            _metricService.IncrementCounter(LAMBDA_INVOCATION_COUNT, "function_name", lambda.function, (*_lambdaCounterMap)[lambda.arn].invocations);
            _metricService.SetGauge(LAMBDA_INSTANCES_COUNT, "function_name", lambda.function, static_cast<double>(lambda.instances.size()));
            _metricService.SetGauge(LAMBDA_INVOCATION_TIMER, "function_name", lambda.function, averageRuntime);
            (*_lambdaCounterMap)[lambda.arn].invocations = 0;
            (*_lambdaCounterMap)[lambda.arn].averageRuntime = 0.0;
        }
        log_trace << "Lambda monitoring finished";
    }

    void LambdaServer::CreateContainers() const {
        try {

            // Get the lambda list
            const Database::Entity::Lambda::LambdaList lambdas = _lambdaDatabase.ListLambdas();
            if (lambdas.empty()) {
                return;
            }

            // Loop over lambdas and create the containers
            log_info << "Start creating lambda functions, count: " << lambdas.size();
            for (const auto &lambda: lambdas) {
                Dto::Lambda::CreateFunctionRequest request;
                request.region = _region;
                request.functionName = lambda.function;
                request.runtime = lambda.runtime;
                Dto::Lambda::CreateFunctionResponse response = _lambdaService.CreateFunction(request);
                log_debug << "Lambda containers created, function: " << lambda.function;
            }
            log_debug << "Lambda containers created, count: " << lambdas.size();

        } catch (Core::ServiceException &e) {
            log_error << e.message();
        }
    }

}// namespace AwsMock::Service
