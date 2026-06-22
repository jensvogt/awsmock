//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/gamma/GammaService.h>

namespace Awsmock::Service {

    void GammaService::UpdateGammaRuntimeStatus(const std::string &region, const Dto::Gamma::GammaStatus &status) const {
        log_debug << "GRT status update, function: " << status.functionName << ", status: " << runtimeStatusToString(status.runtimeStatus) << ", port: " << status.port;

        if (!_gammaDatabase->gammaExists(status.functionName)) {
            log_warning << "GRT status update: gamma not found, function: " << status.functionName;
            return;
        }

        Database::Entity::Gamma::Gamma gamma = _gammaDatabase->getGammaByName(region, status.functionName);

        // Update the matching instance status by public port
        for (auto &instance: gamma.instances) {
            if (instance.publicPort == status.port) {
                switch (status.runtimeStatus) {
                    case RuntimeStatus::idle:
                        instance.status = Database::Entity::Gamma::InstanceIdle;
                        break;
                    case RuntimeStatus::running:
                        instance.status = Database::Entity::Gamma::InstanceRunning;
                        break;
                    case RuntimeStatus::failed:
                        instance.status = Database::Entity::Gamma::InstanceFailed;
                        break;
                    default:
                        break;
                }
                break;
            }
        }

        // Propagate aggregate stats to the gamma entity
        gamma.invocations = status.invocations;
        gamma.averageRuntime = static_cast<long>(status.avgDuration);
        _gammaDatabase->updateGamma(gamma);
        log_debug << "GRT status applied, function: " << status.functionName << ", invocations: " << status.invocations;
    }

}// namespace Awsmock::Service
