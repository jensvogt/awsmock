//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/lambda/LambdaServer.h>

namespace AwsMock::Service {
    LambdaServer::LambdaServer(Core::PeriodicScheduler &scheduler) : AbstractServer("lambda"), _lambdaDatabase(Database::LambdaDatabase::instance()) {

        const Core::Configuration &configuration = Core::Configuration::instance();
        _monitoringPeriod = configuration.GetValueInt("awsmock.modules.lambda.monitoring.period");
        _counterPeriod = configuration.GetValueInt("awsmock.modules.lambda.counter.period");
        _removePeriod = configuration.GetValueInt("awsmock.modules.lambda.remove.period");

        // Directories
        _lambdaDir = configuration.GetValueString("awsmock.modules.lambda.data-dir");
        log_debug << "Lambda directory: " << _lambdaDir;

        // Create environment
        _region = configuration.GetValueString("awsmock.region");

        // Create lambda directory
        Core::DirUtils::EnsureDirectory(_lambdaDir);

        // Cleanup container
        //CleanupContainers();

        // Cleanup instances
        CleanupInstances();

        // Create a local network, if it is not existing yet
        CreateLocalNetwork();

        // Start the lambdas, this will build the containers, if not already existing
        CreateContainers();

        // Start lambda monitoring update counters
        scheduler.AddTask("monitoring-lambda-counters", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Start delete old message task
        scheduler.AddTask("lambda-remove-lambdas", [this] { RemoveExpiredLambdas(); }, _removePeriod);

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
        log_debug << "All lambda instances cleaned up";
    }

    void LambdaServer::CleanupContainers() const {
        _dockerService.PruneContainers();
        log_debug << "Docker containers cleaned up";
    }

    void LambdaServer::CleanupInstances() const {
        log_debug << "Cleanup lambdas";

        for (std::vector<Database::Entity::Lambda::Lambda> lambdas = _lambdaDatabase.ListLambdas(_region); auto &lambda: lambdas) {
            log_debug << "Get containers";
            for (std::vector<Dto::Docker::Container> containers = _dockerService.ListContainerByImageName(lambda.function, "latest"); const auto &container: containers) {
                ContainerService::instance().StopContainer(container.id);
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

        // Get lambda list
        Database::Entity::Lambda::LambdaList lambdaList = _lambdaDatabase.ListLambdas();
        if (lambdaList.empty()) {
            return;
        }
        log_debug << "Lambda worker starting, count: " << lambdaList.size();

        // Get lifetime from configuration
        const int lifetime = Core::Configuration::instance().GetValueInt("awsmock.modules.lambda.lifetime");
        const auto expired = system_clock::now() - std::chrono::seconds(lifetime);

        // Loop over lambdas and remove expired instances
        for (auto &lambda: lambdaList) {

            if (lambda.instances.empty()) {
                continue;
            }

            std::vector<Database::Entity::Lambda::Instance> toBeRemoved;
            for (const auto &instance: lambda.instances) {
                if (instance.status == Database::Entity::Lambda::InstanceIdle && instance.created < expired) {
                    log_info << "Lambda instance expired, function: " << lambda.function << " containerId: " << instance.containerId;
                    _dockerService.StopContainer({.id = instance.containerId});
                    toBeRemoved.emplace_back(instance);
                }
            }
            if (!toBeRemoved.empty()) {
                for (const auto &instance: toBeRemoved) {
                    lambda.RemoveInstance(instance);
                }
                lambda = _lambdaDatabase.UpdateLambda(lambda);
                log_debug << "Lambda updated, function" << lambda.function << " removed: " << toBeRemoved.size();
                _dockerService.PruneContainers();
            }
        }
        log_debug << "Lambda worker finished, count: " << lambdaList.size();
    }

    void LambdaServer::UpdateCounter() const {
        log_trace << "Lambda monitoring starting";

        const long lambdas = _lambdaDatabase.LambdaCount();
        _metricService.SetGauge(LAMBDA_FUNCTION_COUNT, lambdas);

        log_trace << "Lambda monitoring finished";
    }

    void LambdaServer::CreateContainers() const {

        // Get lambda list
        const Database::Entity::Lambda::LambdaList lambdaList = _lambdaDatabase.ListLambdas();
        if (lambdaList.empty()) {
            return;
        }

        // Loop over lambdas and create the containers
        for (auto &lambda: lambdaList) {
            Dto::Lambda::CreateFunctionRequest request = {{.region = _region}, lambda.function, lambda.runtime};
            Dto::Lambda::CreateFunctionResponse response = _lambdaService.CreateFunction(request);
            log_debug << "Lambda containers created, function: " << lambda.function;
        }
        log_debug << "Lambda containers created, count: " << lambdaList.size();
    }

}// namespace AwsMock::Service
