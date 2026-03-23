//
// Created by vogje01 on 01/05/2023.
//

#include <awsmock/service/monitoring/MetricDockerCollector.h>

namespace AwsMock::Monitoring {

    void MetricDockerCollector::CollectDockerCounter() {
        log_trace << "Docker collector starting";

        // Get the container list
        if (const std::vector<Dto::Docker::Container> containers = Service::ContainerService::instance().ListContainers().containerList; !containers.empty()) {

            for (const auto &container: containers) {

                // Sanitize name
                std::string containerName = container.image;
                if (Core::StringUtils::Contains(containerName, ":")) {
                    containerName = containerName.substr(0, containerName.find(':'));
                }
                if (Core::StringUtils::StartsWith(containerName, "/")) {
                    containerName = containerName.substr(1);
                }

                // Get statistics
                const Dto::Docker::ContainerStat stats = Service::ContainerService::instance().GetContainerStats(container.id);

                // CPU
                if (const double timeDiff = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(stats.read - stats.preRead).count()); timeDiff > 0) {
                    const auto numCpus = static_cast<double>(stats.cpuStats.onlineCpus);
                    const auto cpuDelta = static_cast<double>(stats.cpuStats.cpuUsage.total - stats.preCpuStats.cpuUsage.total);
                    if (numCpus > 0) {
                        const auto cpuPercent = cpuDelta / timeDiff / numCpus * 100.0;
                        Core::EventBus::instance().sigMetricGauge(DOCKER_CPU_TOTAL, "container", containerName, static_cast<double>(containers.size()));
                    }

                    // Memory
                    const auto availableMem = static_cast<double>(stats.memoryStats.limit);
                    const auto usedMem = static_cast<double>(stats.memoryStats.usage - stats.memoryStats.stats.cache);
                    const auto memPercent = usedMem / availableMem * 100.0;
                    Core::EventBus::instance().sigMetricGauge(DOCKER_MEMORY_TOTAL, "container", containerName, memPercent);
                }
            }
            Core::EventBus::instance().sigMetricGauge(DOCKER_CONTAINER_COUNT, {}, {}, static_cast<double>(containers.size()));
        }
    }

}// namespace AwsMock::Monitoring
