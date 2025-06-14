//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/ssm/SSMServer.h>

namespace AwsMock::Service {

    SSMServer::SSMServer(Core::Scheduler &scheduler) : AbstractServer("kms") {

        // HTTP manager configuration
        _workerPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.ssm.worker.period");
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.ssm.monitoring.period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.transfer.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.backup.cron");
        log_debug << "SSM server initialized";

        // Check module active
        if (!IsActive("ssm")) {
            log_info << "SSM module inactive";
            return;
        }
        log_info << "SSM server starting";

        // Monitoring
        // Start SNS monitoring update counters
        scheduler.AddTask("ssm-monitoring", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("ssm-backup", [this] { BackupSsm(); }, _backupCron);
        }

        // Set running
        SetRunning();

        log_debug << "SSM server started, workerPeriod: " << _workerPeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

    void SSMServer::UpdateCounter() const {
        log_trace << "SSM monitoring starting";

        // Get total counts
        const long parameters = _ssmDatabase.CountParameters();
        _metricService.SetGauge(SSM_PARAMETER_COUNT, {}, {}, parameters);

        log_trace << "SSM monitoring finished";
    }

    void SSMServer::BackupSsm() {
        ModuleService::BackupModule("ssm", true);
    }

}// namespace AwsMock::Service
