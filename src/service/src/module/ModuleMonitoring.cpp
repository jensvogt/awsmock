//
// Created by vogje01 on 6/3/24.
//

#include <awsmock/service/module/ModuleMonitoring.h>

namespace AwsMock::Service {

    ModuleMonitoring::ModuleMonitoring(int timeout) : Core::Timer("manager-monitoring", timeout) {
        Start();
    }

    void ModuleMonitoring::Initialize() {
        UpdateCounter();
    }

    void ModuleMonitoring::Run() {
        UpdateCounter();
    }

    void ModuleMonitoring::Shutdown() {}

    void ModuleMonitoring::UpdateCounter() {

        using namespace Database::Entity::Module;

        log_trace << "Manager monitoring starting";

        Monitoring::MetricService &metricService = Monitoring::MetricService::instance();
        Database::Entity::Module::ModuleList modules = _moduleDatabase.ListModules();
        for (const auto &module: modules) {
            if (module.status == ModuleStatus::ACTIVE && module.state == ModuleState::RUNNING) {
                metricService.SetGauge(MODULE_UPDOWN_GAUGE, "module", module.name, 0.0);
            } else if (module.status == ModuleStatus::ACTIVE && module.state == ModuleState::STOPPED) {
                metricService.SetGauge(MODULE_UPDOWN_GAUGE, "module", module.name, 2.0);
            } else if (module.status == ModuleStatus::ACTIVE && module.state == ModuleState::STOPPED) {
                metricService.SetGauge(MODULE_UPDOWN_GAUGE, "module", module.name, 0.0);
            } else if (module.status == ModuleStatus::INACTIVE && module.state == ModuleState::RUNNING) {
                metricService.SetGauge(MODULE_UPDOWN_GAUGE, "module", module.name, 1.0);
            }
        }
        log_trace << "Manager monitoring finished";
    }
}// namespace AwsMock::Service
