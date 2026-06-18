//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/kms/KMSServer.h>

namespace Awsmock::Service {

    KMSServer::KMSServer() : AbstractServer("kms") {

        // HTTP manager configuration
        _removePeriod = Core::Configuration::instance().get<int>("awsmock.modules.kms.remove-period");
        _monitoringPeriod = Core::Configuration::instance().get<int>("awsmock.modules.kms.monitoring-period");
        _backupActive = Core::Configuration::instance().get<bool>("awsmock.modules.kms.backup.active");
        _backupCron = Core::Configuration::instance().get<std::string>("awsmock.modules.kms.backup.cron");
        log_debug << "KMS server initialized";

        // Start lambda monitoring update counters
        Core::Scheduler::instance().AddTask("kms-monitoring", [this] { this->updateCounter(); }, _monitoringPeriod);

        // Start the deleting old keys task
        Core::Scheduler::instance().AddTask("kms-delete-keys", [this] { this->deleteKeys(); }, _removePeriod);

        // Start backup
        if (_backupActive) {
            Core::Scheduler::instance().AddTask("kms-backup", [] { backupKms(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&KMSServer::shutdown, this));

        log_debug << "KMS server initialized, removePeriod: " << _removePeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

    void KMSServer::updateCounter() const {
        log_trace << "KMS monitoring starting";

        // Get total counts
        const long keys = _kmsDatabase->countKeys();
        Core::EventBus::instance().sigMetricGauge(KMS_KEY_COUNT, {}, {}, keys);

        log_trace << "KMS monitoring finished";
    }

    void KMSServer::deleteKeys() const {
        log_trace << "Starting delete keys";

        for (const auto &key: _kmsDatabase->listKeys({}, {}, 0, 0, {})) {
            if (key.keyState == Dto::KMS::KeyStateToString(Dto::KMS::KeyState::PENDING_DELETION) && key.scheduledDeletion < system_clock::now()) {
                _kmsDatabase->deleteKey(key);
                log_debug << "Key deleted, keyId: " << key.keyId;
            }
        }
        log_trace << "Finished key deletion";
    }

    void KMSServer::backupKms() {
        ModuleService{}.BackupModule("kms", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void KMSServer::shutdown() {
        log_debug << "KMS server shutdown";
        Core::Scheduler::instance().Shutdown("kms-monitoring");
        Core::Scheduler::instance().Shutdown("kms-delete-keys");
        Core::Scheduler::instance().Shutdown("kms-backup");
        log_info << "KMS server stopped";
    }
}// namespace Awsmock::Service
