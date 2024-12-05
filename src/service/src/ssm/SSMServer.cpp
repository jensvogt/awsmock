//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/ssm/SSMServer.h>

namespace AwsMock::Service {

    SSMServer::SSMServer(Core::PeriodicScheduler &scheduler) : AbstractServer("kms") {

        // HTTP manager configuration
        Core::Configuration &configuration = Core::Configuration::instance();
        _workerPeriod = configuration.GetValueInt("awsmock.modules.ssm.worker.period");
        _monitoringPeriod = configuration.GetValueInt("awsmock.modules.ssm.monitoring.period");
        log_debug << "SSM server initialized";

        // Check module active
        if (!IsActive("ssm")) {
            log_info << "SSM module inactive";
            return;
        }
        log_info << "SSM server starting";

        // Monitoring
        // Start SNS monitoring update counters
        scheduler.AddTask("monitoring-ssm-counters", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Start delete old message task
        //scheduler.AddTask("s3-sync-directories", [this] { this->_ssmWorker.SyncObjects(); }, _workerPeriod);

        // Set running
        SetRunning();

        log_debug << "SSM server started, workerPeriod: " << _workerPeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

    void SSMServer::UpdateCounter() const {
        log_trace << "SSM monitoring starting";

        // Get total counts
        const long parameters = _ssmDatabase.CountParameters();
        _metricService.SetGauge(SSM_PARAMETER_COUNT, parameters);

        log_trace << "SSM monitoring finished";
    }

}// namespace AwsMock::Service
