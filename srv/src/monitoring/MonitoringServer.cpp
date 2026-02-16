//
// Created by vogje01 on 04/01/2023.
//

#include <awsmock/service/monitoring/MonitoringServer.h>

namespace AwsMock::Service {

    MonitoringServer::MonitoringServer(Core::Scheduler &scheduler) : AbstractServer("monitoring"), _scheduler(scheduler) {

        Monitoring::MetricService::instance().Initialize();
        const int systemPeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.system-period");
        const int averagePeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.average-period");
        const int retentionPeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.retention");

        // Start monitoring system collector
        _scheduler.AddTask("monitoring-system-collector", [this] { this->_metricSystemCollector.CollectSystemCounter(); }, systemPeriod);
        log_debug << "System collector started";

        _scheduler.AddTask("monitoring-docker-collector", [] { CollectDockerCounter(); }, systemPeriod);
        log_debug << "System collector started";

        _scheduler.AddTask("monitoring-collector", [this] { this->Collector(); }, averagePeriod);
        log_debug << "System collector started";

        // Start the database cleanup worker thread every day
        _scheduler.AddTask("monitoring-cleanup-database", [this] { this->DeleteMonitoringData(); }, retentionPeriod * 24 * 3600, Core::DateTimeUtils::GetSecondsUntilMidnight());
        log_debug << "Cleanup started";

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&MonitoringServer::Shutdown, this));

        log_debug << "Monitoring module initialized";
    }

    void MonitoringServer::DeleteMonitoringData() const {

        log_trace << "Monitoring worker starting";

        const int retentionPeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.retention");
        const long deletedCount = _monitoringDatabase.DeleteOldMonitoringData(retentionPeriod);

        log_trace << "Monitoring worker finished, retentionPeriod: " << retentionPeriod << " deletedCount: " << deletedCount;
    }

    void MonitoringServer::CollectDockerCounter() {

        // Get the container list
        if (const std::vector<Dto::Docker::Container> containers = ContainerService::instance().ListContainers().containerList; !containers.empty()) {

            for (const auto &container: containers) {

                // Sanitize name
                std::string containerName = container.image;
                if (Core::StringUtils::Contains(containerName, ":")) {
                    containerName = containerName.substr(0, containerName.find(':'));
                }

                // Get statistics
                const Dto::Docker::ContainerStat stats = ContainerService::instance().GetContainerStats(container.id);

                // CPU
                const double timeDiff = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(stats.read - stats.preRead).count());

                const auto numCpus = static_cast<double>(stats.cpuStats.onlineCpus);
                const auto cpuDelta = static_cast<double>(stats.cpuStats.cpuUsage.total - stats.preCpuStats.cpuUsage.total);
                if (timeDiff > 0 && numCpus > 0) {
                    const auto cpuPercent = cpuDelta / timeDiff / numCpus * 100.0;
                    Core::MonitoringCollector::instance().SetGauge(DOCKER_CPU_TOTAL, "container", containerName, cpuPercent);
                }

                // Memory
                const auto availableMem = static_cast<double>(stats.memoryStats.limit);
                const auto usedMem = static_cast<double>(stats.memoryStats.usage - stats.memoryStats.stats.cache);
                const auto memPercent = usedMem / availableMem * 100.0;
                Core::MonitoringCollector::instance().SetGauge(DOCKER_MEMORY_TOTAL, "container", containerName, memPercent);
            }
            Core::MonitoringCollector::instance().SetGauge(DOCKER_CONTAINER_COUNT, static_cast<long>(containers.size()));
        }
    }

    void MonitoringServer::Collector() const {
        _monitoringDatabase.UpdateMonitoringCounters();
    }

    void MonitoringServer::Shutdown() {
        log_debug << "Monitoring server shutdown";
        _scheduler.Shutdown("monitoring-system-collector");
        _scheduler.Shutdown("monitoring-docker-collector");
        _scheduler.Shutdown("monitoring-collector");
        _scheduler.Shutdown("monitoring-cleanup-database");
        log_info << "Monitoring server stopped";
    }

}// namespace AwsMock::Service
