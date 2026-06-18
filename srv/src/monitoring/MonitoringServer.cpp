//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/monitoring/MonitoringServer.h>

namespace Awsmock::Service {
    MonitoringServer::MonitoringServer() : AbstractServer("monitoring") {
        const int systemPeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.system-period");
        const int retentionPeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.retention");
        const int averagePeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.average-period");

        // Start monitoring system collector
        Core::Scheduler::instance().AddTask("monitoring-system-collector", [this] { _metricSystemCollector.CollectSystemCounter(); }, systemPeriod);
        log_debug << "System collector started";

        Core::Scheduler::instance().AddTask("monitoring-docker-collector", [] { Monitoring::MetricDockerCollector{}.CollectDockerCounter(); }, systemPeriod);
        log_debug << "Docker collector started";

        // Start the metric aggregation and emit cycle
        Core::Scheduler::instance().AddTask("monitoring-average-collector", [this] { _monitoringCollector.Collect(); }, averagePeriod);
        log_debug << "Average collector started";

        // Start the database cleanup worker thread every day
        Core::Scheduler::instance().AddTask("monitoring-cleanup-database", [this] { deleteMonitoringData(); }, retentionPeriod * 24 * 3600, Core::DateTimeUtils::GetSecondsUntilMidnight());
        log_debug << "Cleanup started";

        // Load exclusions
        for (const auto &e: Core::Configuration::instance().getArray<std::string>("awsmock.monitoring.exclusions")) {
            _exclusions.push_back(e);
        }

        // Connect stop signal
        _shutdownConnection = Core::EventBus::instance().sigShutdown.connect([this]() {
            this->shutdown();
        });

        // Connect monitoring signal
        _metricGaugeConnection = Core::EventBus::instance().sigMetricGauge.connect([this](const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) {
            if (checkExclusions(name, labelName, labelValue)) {
                this->_monitoringCollector.SetGauge(name, labelName, labelValue, value);
            }
        });
        _metricRateConnection = Core::EventBus::instance().sigMetricRate.connect([this](const std::string &name, const std::string &labelName, const std::string &labelValue) {
            if (checkExclusions(name, labelName, labelValue)) {
                this->_monitoringCollector.Increment(name, labelName, labelValue);
            }
        });

        log_debug << "Monitoring module initialized";
    }

    void MonitoringServer::deleteMonitoringData() const {

        log_trace << "Monitoring worker starting";

        const int retentionPeriod = Core::Configuration::instance().get<int>("awsmock.monitoring.retention");
        const long deletedCount = _monitoringDatabase->deleteOldMonitoringData(retentionPeriod);

        log_trace << "Monitoring worker finished, retentionPeriod: " << retentionPeriod << " deletedCount: " << deletedCount;
    }

    void MonitoringServer::shutdown() {
        log_debug << "Monitoring server shutdown";
        Core::Scheduler::instance().Shutdown("monitoring-system-collector");
        Core::Scheduler::instance().Shutdown("monitoring-docker-collector");
        Core::Scheduler::instance().Shutdown("monitoring-average-collector");
        Core::Scheduler::instance().Shutdown("monitoring-cleanup-database");
        log_info << "Monitoring server stopped";
    }

    bool MonitoringServer::checkExclusions(const std::string &name, const std::string &labelName, const std::string &labelValue) const {
        if (_exclusions.empty()) {
            return true;
        }
        return std::ranges::find(_exclusions, name + "::" + labelName + "::" + labelValue) == _exclusions.end();
    }

}// namespace Awsmock::Service
