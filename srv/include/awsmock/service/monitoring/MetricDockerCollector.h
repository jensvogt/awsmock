//
// Created by vogje01 on 01/05/2023.
//

#ifndef AWSMOCK_MONITORING_METRIC_DOCKER_COLLECTOR_H
#define AWSMOCK_MONITORING_METRIC_DOCKER_COLLECTOR_H

// Awsmock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/dto/container/model/Container.h>
#include <awsmock/service/container/ContainerService.h>

namespace AwsMock::Monitoring {

    /**
     * @brief Collect docker information like CPU and Memory.
     *
     * Runs as a background thread with a given timeout in ms.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MetricDockerCollector {

      public:

        /**
         * @brief Constructor.
         */
        explicit MetricDockerCollector() = default;

        /**
         * @brief Updates the system counter
         */
        static void CollectDockerCounter();
    };

}// namespace AwsMock::Monitoring

#endif// AWSMOCK_MONITORING_METRIC_DOCKER_COLLECTOR_H
