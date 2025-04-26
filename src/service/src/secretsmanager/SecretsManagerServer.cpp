//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/secretsmanager/SecretsManagerServer.h>

namespace AwsMock::Service {

    SecretsManagerServer::SecretsManagerServer(Core::PeriodicScheduler &scheduler) : AbstractServer("secretsmanager") {

        // HTTP manager configuration
        const Core::Configuration &configuration = Core::Configuration::instance();
        _monitoringPeriod = configuration.GetValue<int>("awsmock.modules.secretsmanager.monitoring.period");
        log_debug << "SecretsManager rest module initialized";

        // Check module active
        if (!IsActive("secretsmanager")) {
            log_info << "SecretsManager module inactive";
            return;
        }
        log_info << "SecretsManager server starting";

        // Start secrets manager monitoring update counters
        scheduler.AddTask("monitoring-sns-counters", [this] { this->_secretsManagerMonitoring.UpdateCounter(); }, _monitoringPeriod);

        // Set running
        SetRunning();
    }

}// namespace AwsMock::Service
