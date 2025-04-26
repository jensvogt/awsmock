//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/kms/KMSServer.h>

namespace AwsMock::Service {

    KMSServer::KMSServer(Core::PeriodicScheduler &scheduler) : AbstractServer("kms"), _kmsDatabase(Database::KMSDatabase::instance()) {

        // HTTP manager configuration
        Core::Configuration &configuration = Core::Configuration::instance();
        _removePeriod = configuration.GetValue<int>("awsmock.modules.kms.remove.period");
        _monitoringPeriod = configuration.GetValue<int>("awsmock.modules.kms.monitoring.period");
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
        scheduler.AddTask("lambda-remove-lambdas", [this] { this->_kmsWorker.DeleteKeys(); }, _removePeriod);

        // Set running
        SetRunning();

        log_debug << "KMSServer initialized, workerPeriod: " << _removePeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

}// namespace AwsMock::Service
