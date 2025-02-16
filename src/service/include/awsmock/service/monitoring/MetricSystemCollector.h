//
// Created by vogje01 on 01/05/2023.
//

#ifndef AWSMOCK_MONITORING_METRIC_SYSTEM_COLLECTOR_H
#define AWSMOCK_MONITORING_METRIC_SYSTEM_COLLECTOR_H

// C includes
#include <sys/times.h>

// C++ Standard includes
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

#ifdef __linux__
#include <sys/sysinfo.h>
#endif

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/resource.h>
#endif

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Monitoring {

    /**
     * @brief Collect system information like CPU and Memory.
     *
     * Runs as background thread with a given timeout in ms.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MetricSystemCollector {

      public:

        /**
         * @brief Constructor.
         */
        explicit MetricSystemCollector();

        /**
         * @brief Updates the system counter
         */
        void CollectSystemCounter();

#ifdef __APPLE__

        /**
        * @brief Get number of threads on MacOS
        */
        void GetCurrentThreadCountMac();

        /**
         * @brief Get CPU utilization on MacOS
         */
        void GetCpuInfoMac();

        /**
         * @brief Get memory utilization on MacOS
         */
        void GetMemoryInfoMac();

#elif __linux__

        /**
         * @brief Get number of threads on MacOS
         */
        void GetCurrentThreadCountLinux();

        /**
         * @brief Get CPU utilization on MacOS
        */
        void GetCpuInfoLinux();

        /**
         * @brief Get memory info on Linux systems
         */
        void GetMemoryThreadsInfoLinux();
#endif

      private:

        /**
         * Start time
         */
        system_clock::time_point _startTime;

        /**
         * Monitoring period
         */
        int _period = 60;
    };

}// namespace AwsMock::Monitoring

#endif// AWSMOCK_MONITORING_METRIC_SYSTEM_COLLECTOR_H
