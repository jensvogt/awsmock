//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/monitoring/MonitoringServer.h>

namespace AwsMock::Service {

    MonitoringServer::MonitoringServer(Core::PeriodicScheduler &scheduler) : AbstractServer("monitoring") {

        Monitoring::MetricService::instance().Initialize();
        const int period = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.period");

        // Start monitoring system collector
        scheduler.AddTask("monitoring-system-collector", [this] { this->_metricSystemCollector.CollectSystemCounter(); }, period);
        log_debug << "System collector started";

        // Start the database cleanup worker thread every day
        scheduler.AddTask("monitoring-cleanup-database", [this] { this->DeleteMonitoringData(); }, 24 * 3600, Core::DateTimeUtils::GetSecondsUntilMidnight());
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

}// namespace AwsMock::Service
