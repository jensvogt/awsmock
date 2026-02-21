//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/ssm/SSMServer.h>

#include "awsmock/core/EventBus.h"

namespace AwsMock::Service {

    SSMServer::SSMServer(Core::Scheduler &scheduler) : AbstractServer("ssm"), _scheduler(scheduler) {

        // HTTP manager configuration
        _workerPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.ssm.worker-period");
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.ssm.monitoring-period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.transfer.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.backup.cron");
        log_debug << "SSM server initialized";

        // Start SNS monitoring update counters
        _scheduler.AddTask("ssm-monitoring", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            _scheduler.AddTask("ssm-backup", [this] { BackupSsm(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&SSMServer::Shutdown, this));

        log_debug << "SSM server started";
    }

    void SSMServer::UpdateCounter() const {
        log_trace << "SSM monitoring starting";

        // Get total counts
        const long parameters = _ssmDatabase.CountParameters();
        // TODO: fix and use new monitoring counters
        //_metricService.SetGauge(SSM_PARAMETER_COUNT, {}, {}, parameters);

        log_trace << "SSM monitoring finished";
    }

    void SSMServer::BackupSsm() {
        ModuleService::BackupModule("ssm", true);
    }

    void SSMServer::Shutdown() {
        log_info << "SSM manager server shutting down";
        _scheduler.Shutdown("ssm-monitoring");
        _scheduler.Shutdown("ssm-backup");
    }
} // namespace AwsMock::Service
