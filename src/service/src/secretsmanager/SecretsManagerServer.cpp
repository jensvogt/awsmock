//
// Created by vogje01 on 03/06/2023.
//

#include "../../../core/include/awsmock/core/monitoring/MonitoringDefinition.h"


#include <awsmock/service/secretsmanager/SecretsManagerServer.h>

namespace AwsMock::Service {

    SecretsManagerServer::SecretsManagerServer(Core::Scheduler &scheduler) : AbstractServer("secretsmanager") {

        // Manager configuration
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.secretsmanager.monitoring.period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.secretsmanager.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.secretsmanager.backup.cron");
        log_debug << "SecretsManager rest module initialized";

        // Check module active
        if (!IsActive("secretsmanager")) {
            log_info << "SecretsManager module inactive";
            return;
        }
        log_info << "SecretsManager server starting";

        // Start secrets manager monitoring update counters
        scheduler.AddTask("secretsmanager-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("secretsmanager-backup", [] { BackupSecretsManger(); }, _backupCron);
        }

        // Set running
        SetRunning();
    }

    void SecretsManagerServer::UpdateCounter() const {
        const long secrets = _secretsManagerDatabase.CountSecrets();
        _metricService.SetGauge(SECRETSMANAGER_SECRETS_COUNT, {}, {}, static_cast<double>(secrets));
        log_trace << "Secrets manager update counter finished";
    }

    void SecretsManagerServer::BackupSecretsManger() {
        ModuleService::BackupModule("secretsmanager", true);
    }

}// namespace AwsMock::Service
