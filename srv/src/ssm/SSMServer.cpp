//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/ssm/SSMServer.h>

namespace Awsmock::Service {

    SSMServer::SSMServer() : AbstractServer("ssm") {

        // HTTP manager configuration
        _workerPeriod = Core::Configuration::instance().get<int>("awsmock.modules.ssm.worker-period");
        _monitoringPeriod = Core::Configuration::instance().get<int>("awsmock.modules.ssm.monitoring-period");
        _backupActive = Core::Configuration::instance().get<bool>("awsmock.modules.transfer.backup.active");
        _backupCron = Core::Configuration::instance().get<std::string>("awsmock.modules.transfer.backup.cron");
        log_debug << "SSM server initialized";

        // Start SNS monitoring update counters
        Core::Scheduler::instance().AddTask("ssm-monitoring", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Start backup
        if (_backupActive) {
            Core::Scheduler::instance().AddTask("ssm-backup", [this] { BackupSsm(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&SSMServer::shutdown, this));

        log_debug << "SSM server started";
    }

    void SSMServer::UpdateCounter() const {
        log_trace << "SSM monitoring starting";

        // Get total counts
        const long parameters = _ssmDatabase->countParameters("", "");
        // TODO: fix and use new monitoring counters
        //_metricService.SetGauge(SSM_PARAMETER_COUNT, {}, {}, parameters);

        log_trace << "SSM monitoring finished";
    }

    void SSMServer::BackupSsm() {
        ModuleService{}.BackupModule("ssm", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void SSMServer::shutdown() {
        log_info << "SSM manager server shutting down";
        Core::Scheduler::instance().Shutdown("ssm-monitoring");
        Core::Scheduler::instance().Shutdown("ssm-backup");
    }
}// namespace Awsmock::Service
