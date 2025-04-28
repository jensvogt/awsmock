//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/ssm/SSMServer.h>

namespace AwsMock::Service {

    SSMServer::SSMServer(Core::PeriodicScheduler &scheduler) : AbstractServer("kms") {

        // HTTP manager configuration
        const Core::Configuration &configuration = Core::Configuration::instance();
        _workerPeriod = configuration.GetValue<int>("awsmock.modules.ssm.worker.period");
        _monitoringPeriod = configuration.GetValue<int>("awsmock.modules.ssm.monitoring.period");
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
