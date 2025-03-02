//
// Created by vogje01 on 01/05/2023.
//

#ifndef AWSMOCK_MONITORING_METRIC_SYSTEM_COLLECTOR_H
#define AWSMOCK_MONITORING_METRIC_SYSTEM_COLLECTOR_H

// C includes
#ifndef _WIN32
#include <sys/times.h>
#endif
#ifdef __linux__
#include <sys/sysinfo.h>
#elif __APPLE__
#include <mach/mach.h>
#include <sys/resource.h>
#endif
#ifdef _WIN32
//#include "psapi.h"
#endif

// C++ Standard includes
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

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
        void GetThreadInfoMac();

        /**
         * @brief Get CPU utilization on MacOS
         */
        void GetCpuInfoMac();

        /**
         * @brief Get memory utilization on MacOS
         */
        static void GetMemoryInfoMac();

#elif __linux__

        /**
         * @brief Get number of threads on MacOS
         */
        static void GetThreadInfoLinux();

        /**
         * @brief Get CPU utilization on MacOS
        */
        void GetCpuInfoLinux();

        /**
         * @brief Get memory info on Linux systems
         */
        static void GetMemoryInfoLinux();

#elif _WIN32

        /**
         * @brief Get CPU utilization on Windows
         */
        void GetCpuInfoWin32();


        /**
         * @brief Get memory utilization on Win32
         */
        static void GetMemoryInfoWin32();

#endif

      private:

#ifdef __linux__
        clock_t _lastTime = 0;
        clock_t _lastTotalCPU = 0;
        clock_t _lastSysCPU = 0;
        clock_t _lastUserCPU = 0;
#elif _WIN32
        ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        int numProcessors;
        HANDLE self;
#endif

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
