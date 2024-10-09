//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/kms/KMSServer.h>

namespace AwsMock::Service {

    KMSServer::KMSServer(Core::PeriodicScheduler &scheduler) : AbstractServer("kms", 10), _kmsDatabase(Database::KMSDatabase::instance()) {

        // HTTP manager configuration
        Core::Configuration &configuration = Core::Configuration::instance();
        _workerPeriod = configuration.getInt("awsmock.service.kms.worker.period", KMS_DEFAULT_WORKER_PERIOD);
        _monitoringPeriod = configuration.getInt("awsmock.service.kms.monitoring.period", KMS_DEFAULT_MONITORING_PERIOD);
        log_debug << "KMS server initialized";

        // Check module active
        if (!IsActive("sns")) {
            log_info << "KMS module inactive";
            return;
        }
        log_info << "KMS module starting";

        // Start lambda monitoring update counters
        scheduler.AddTask("monitoring-lambda-counters", [this] { this->_kmsMonitoring.UpdateCounter(); }, _monitoringPeriod);

        // Start delete old keys
        scheduler.AddTask("lambda-remove-lambdas", [this] { this->_kmsWorker.DeleteKeys(); }, _workerPeriod);

        // Set running
        SetRunning();

        log_debug << "KMSServer initialized, workerPeriod: " << _workerPeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

    void KMSServer::Initialize() {
    }

    void KMSServer::Run() {
        // Intentionally left empty
    }

    void KMSServer::Shutdown() {
    }

}// namespace AwsMock::Service
