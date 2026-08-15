//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/secretsmanager/SecretsManagerServer.h>

namespace Awsmock::Service {

    SecretsManagerServer::SecretsManagerServer() : AbstractServer("secretsmanager") {

        // Manager configuration
        const int monitoringPeriod = Core::Configuration::instance().get<int>("awsmock.modules.secretsmanager.monitoring-period");
        const int rotationCheckPeriod = Core::Configuration::instance().getOr<int>("awsmock.modules.secretsmanager.rotation-check-period", SECRETSMANAGER_DEFAULT_ROTATION_CHECK_PERIOD);
        const bool backupActive = Core::Configuration::instance().get<bool>("awsmock.modules.secretsmanager.backup.active");
        const auto backupCron = Core::Configuration::instance().get<std::string>("awsmock.modules.secretsmanager.backup.cron");
        log_debug << "SecretsManager rest module initialized";

        // Start secrets manager monitoring update counters
        Core::Scheduler::instance().AddTask("secretsmanager-monitoring", [this] { this->UpdateCounter(); }, monitoringPeriod);

        // Start secret rotation check
        Core::Scheduler::instance().AddTask("secretsmanager-rotation", [this] { this->CheckRotations(); }, rotationCheckPeriod);

        // Start backup
        if (backupActive) {
            Core::Scheduler::instance().AddTask("secretsmanager-backup", [] { BackupSecretsManger(); }, backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&SecretsManagerServer::shutdown, this));

        log_debug << "Secrets manager server initialized";
    }

    void SecretsManagerServer::UpdateCounter() const {
        const long secrets = _secretsManagerDatabase->CountSecrets();
        Core::EventBus::instance().sigMetricGauge(SECRETSMANAGER_SECRETS_COUNT, {}, {}, secrets);
        log_trace << "Secrets manager update counter finished";
    }

    void SecretsManagerServer::BackupSecretsManger() {
        ModuleService{}.BackupModule("secretsmanager", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void SecretsManagerServer::CheckRotations() const {
        log_trace << "Checking secret rotations";

        const system_clock::time_point now = system_clock::now();
        for (auto secret: _secretsManagerDatabase->ListSecrets()) {

            if (!secret.rotationEnabled || secret.rotationLambdaARN.empty() || secret.nextRotatedDate > now) {
                continue;
            }
            log_info << "Secret rotation due, starting rotation, secretId: " << secret.secretId;

            const std::string clientRequestToken = Core::StringUtils::CreateRandomUuid();

            // Register the new version under AWSPENDING, same as the manually triggered rotation
            // in SecretsManagerService::RotateSecret (see there for why the value is left unset).
            secret.lastRotatedDate = now;
            secret.versions[clientRequestToken].stages = {Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSPENDING)};
            secret = _secretsManagerDatabase->UpdateSecret(secret);

            // Passed by value (not boost::ref) so the detached thread owns an independent copy,
            // as this loop continues (and the scheduler task returns) long before the thread's
            // lambda invocations finish.
            boost::thread t(SecretRotation(), secret, clientRequestToken);
            t.detach();
        }
        log_trace << "Secret rotation check finished";
    }

    void SecretsManagerServer::shutdown() {
        log_info << "Secrets manager server shutting down";
        Core::Scheduler::instance().Shutdown("secretsmanager-monitoring");
        Core::Scheduler::instance().Shutdown("secretsmanager-rotation");
        Core::Scheduler::instance().Shutdown("secretsmanager-backup");
    }
}// namespace Awsmock::Service
