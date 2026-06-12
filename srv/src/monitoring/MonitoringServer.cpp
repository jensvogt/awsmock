//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/monitoring/MonitoringServer.h>

namespace Awsmock::Service {
    MonitoringServer::MonitoringServer(Core::Scheduler &scheduler) : AbstractServer("monitoring"), _scheduler(scheduler) {
        const int systemPeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.system-period");
        const int retentionPeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.retention");
        const int averagePeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.average-period");

        // Start monitoring system collector
        _scheduler.AddTask("monitoring-system-collector", [this] { _metricSystemCollector.CollectSystemCounter(); }, systemPeriod);
        log_debug << "System collector started";

        _scheduler.AddTask("monitoring-docker-collector", [] { Monitoring::MetricDockerCollector{}.CollectDockerCounter(); }, systemPeriod);
        log_debug << "Docker collector started";

        // Start the metric aggregation and emit cycle
        _scheduler.AddTask("monitoring-average-collector", [this] { _monitoringCollector.Collect(); }, averagePeriod);
        log_debug << "Average collector started";

        // Start the database cleanup worker thread every day
        _scheduler.AddTask("monitoring-cleanup-database", [this] { DeleteMonitoringData(); }, retentionPeriod * 24 * 3600, Core::DateTimeUtils::GetSecondsUntilMidnight());
        log_debug << "Cleanup started";

        // Load exclusions
        for (const auto &e: Core::Configuration::instance().getArray<std::string>("awsmock.monitoring.exclusions")) {
            _exclusions.push_back(e);
        }

        // Connect stop signal
        _shutdownConnection = Core::EventBus::instance().sigShutdown.connect([this]() {
            this->Shutdown();
        });

        // Connect monitoring signal
        _metricGaugeConnection = Core::EventBus::instance().sigMetricGauge.connect([this](const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) {
            if (CheckExclusions(name, labelName, labelValue)) {
                this->_monitoringCollector.SetGauge(name, labelName, labelValue, value);
            }
        });
        _metricRateConnection = Core::EventBus::instance().sigMetricRate.connect([this](const std::string &name, const std::string &labelName, const std::string &labelValue) {
            if (CheckExclusions(name, labelName, labelValue)) {
                this->_monitoringCollector.Increment(name, labelName, labelValue);
            }
        });

        log_debug << "Monitoring module initialized";
    }

    void MonitoringServer::DeleteMonitoringData() const {

        log_trace << "Monitoring worker starting";

        const int retentionPeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.retention");
        const long deletedCount = _monitoringDatabase->deleteOldMonitoringData(retentionPeriod);

        log_trace << "Monitoring worker finished, retentionPeriod: " << retentionPeriod << " deletedCount: " << deletedCount;
    }

    void MonitoringServer::Shutdown() {
        log_debug << "Monitoring server shutdown";
        _scheduler.Shutdown("monitoring-system-collector");
        _scheduler.Shutdown("monitoring-docker-collector");
        _scheduler.Shutdown("monitoring-average-collector");
        _scheduler.Shutdown("monitoring-cleanup-database");
        log_info << "Monitoring server stopped";
    }

    bool MonitoringServer::CheckExclusions(const std::string &name, const std::string &labelName, const std::string &labelValue) const {
        if (_exclusions.empty()) {
            return true;
        }
        return std::ranges::find(_exclusions, name + "::" + labelName + "::" + labelValue) == _exclusions.end();
    }

} // namespace Awsmock::Service
