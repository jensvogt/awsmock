//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/monitoring/MonitoringServer.h>

namespace AwsMock::Service {

    MonitoringServer::MonitoringServer(Core::Scheduler &scheduler) : AbstractServer("monitoring") {

        Monitoring::MetricService::instance().Initialize();
        const int systemPeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.system-period");
        const int averagePeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.average-period");
        const int retentionPeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.retention");

        // Start monitoring system collector
        scheduler.AddTask("monitoring-system-collector", [this] { this->_metricSystemCollector.CollectSystemCounter(); }, systemPeriod);
        log_debug << "System collector started";

        scheduler.AddTask("monitoring-collector", [this] { this->Collector(); }, averagePeriod);
        log_debug << "System collector started";

        // Start the database cleanup worker thread every day
        scheduler.AddTask("monitoring-cleanup-database", [this] { this->DeleteMonitoringData(); }, retentionPeriod * 24 * 3600, Core::DateTimeUtils::GetSecondsUntilMidnight());
        log_debug << "Cleanup started";

        // Set running
        SetRunning();
        log_debug << "Monitoring module initialized";
    }

    void MonitoringServer::DeleteMonitoringData() const {

        log_trace << "Monitoring worker starting";

        const int retentionPeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.retention");
        const long deletedCount = _monitoringDatabase.DeleteOldMonitoringData(retentionPeriod);

        log_trace << "Monitoring worker finished, retentionPeriod: " << retentionPeriod << " deletedCount: " << deletedCount;
    }

    void MonitoringServer::Collector() const {
        _monitoringDatabase.UpdateMonitoringCounters();
    }

}// namespace AwsMock::Service
