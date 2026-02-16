//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/secretsmanager/SecretsManagerServer.h>

namespace AwsMock::Service {

    SecretsManagerServer::SecretsManagerServer(Core::Scheduler &scheduler) : AbstractServer("secretsmanager"), _scheduler(scheduler) {

        // Manager configuration
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.secretsmanager.monitoring.period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.secretsmanager.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.secretsmanager.backup.cron");
        log_debug << "SecretsManager rest module initialized";

        // Start secrets manager monitoring update counters
        _scheduler.AddTask("secretsmanager-monitoring", [this] { this->UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            _scheduler.AddTask("secretsmanager-backup", [] { BackupSecretsManger(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&SecretsManagerServer::Shutdown, this));

        log_debug << "Secrets manager server initialized";
    }

    void SecretsManagerServer::UpdateCounter() const {
        const long secrets = _secretsManagerDatabase.CountSecrets();
        _metricService.SetGauge(SECRETSMANAGER_SECRETS_COUNT, {}, {}, static_cast<double>(secrets));
        log_trace << "Secrets manager update counter finished";
    }

    void SecretsManagerServer::BackupSecretsManger() {
        ModuleService::BackupModule("secretsmanager", true);
    }

    void SecretsManagerServer::Shutdown() {
        log_info << "Secrets manager server shutting down";
        _scheduler.Shutdown("secretsmanager-monitoring");
        _scheduler.Shutdown("secretsmanager-backup");
    }
}// namespace AwsMock::Service
