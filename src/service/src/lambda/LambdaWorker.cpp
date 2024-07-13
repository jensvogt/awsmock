//
// Created by vogje01 on 4/21/24.
//

#include <awsmock/service/lambda/LambdaWorker.h>

namespace AwsMock::Service {

    void LambdaWorker::Initialize() {

        log_debug << "LambdaWorker initialized";
    }

    void LambdaWorker::Run() {
        RemoveExpiredLambdas();
    }

    void LambdaWorker::Shutdown() {}

    void LambdaWorker::RemoveExpiredLambdas() {

        // Get lambda list
        Database::Entity::Lambda::LambdaList lambdaList = _lambdaDatabase.ListLambdas();
        if (lambdaList.empty()) {
            return;
        }
        log_info << "Lambda worker starting, count: " << lambdaList.size();

        // Get lifetime from configuration
        int lifetime = Core::Configuration::instance().getInt("awsmock.service.lambda.lifetime", DEFAULT_LAMBDA_LIFETIME);
        auto expired = std::chrono::system_clock::now() - std::chrono::minutes(lifetime);

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
            for (const auto &instance: toBeRemoved) {
                lambda.RemoveInstance(instance);
            }
            lambda = _lambdaDatabase.UpdateLambda(lambda);
            log_debug << "Lambda updated, function" << lambda.function << " removed: " << toBeRemoved.size();
        }
        _dockerService.PruneContainers();
        log_info << "Lambda worker finished, count: " << lambdaList.size();
    }

}// namespace AwsMock::Service