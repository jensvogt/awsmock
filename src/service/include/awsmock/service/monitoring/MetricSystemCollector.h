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

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/resource.h>
#endif

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
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
         * @brief Thread counter
         */
        void GetCurrentThreadCount();

        /**
         * @brief CPU info
         */
        void GetCpuInfo();

#endif

      private:

#ifdef __APPLE__
        system_clock::time_point _lastCPUTime;
#endif
        /**
         * Number of processors
         */
        int _numProcessors{};

        /**
         * Last CPU
         */
        clock_t _lastCPU{};

        /**
         * Last system CPU
         */
        clock_t _lastSysCPU{};

        /**
         * Last user CPU
         */
        clock_t _lastUserCPU{};

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
