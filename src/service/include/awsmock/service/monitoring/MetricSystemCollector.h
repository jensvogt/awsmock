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

// C++ Standard includes
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

#ifdef _WIN32
#include "windows.h"
#include <Wbemidl.h>
#include <comdef.h>
#include <pdh.h>
#endif

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
        * @brief Get number of threads on macOS
        */
        void GetThreadInfoMac();

        /**
         * @brief Get CPU utilization on macOS
         */
        void GetCpuInfoMac();

        /**
         * @brief Get memory utilization on macOS
         */
        static void GetMemoryInfoMac();

#elif __linux__

        /**
         * @brief Get number of threads on macOS
         */
        static void GetThreadInfoAwsmockLinux();

        /**
         * @brief Get AwsMock CPU utilization on Linux
         */
        void GetCpuInfoAwsmockLinux();

        /**
         * @brief Get total CPU utilization on Linux
         */
        void GetCpuInfoTotalLinux();

        /**
         * @brief Get AwsMock memory info on Linux systems
         */
        static void GetMemoryInfoAwsmockLinux();

        /**
         * @brief Get AwsMock memory info on Linux systems
         */
        static void GetMemoryInfoTotalLinux();

#elif _WIN32

        /**
         * @brief Get total CPU utilization on Windows
         */
        void GetCpuInfoWin32();

        /**
         * @brief Get CPU utilization on Windows
         */
        void GetCpuInfoAwsmockWin32();

        /**
         * @brief Get total memory utilization on Win32
         */
        void GetMemoryInfoWin32();

        /**
         * @brief Get memory utilization on Win32
         */
        void GetMemoryInfoAwsmockWin32();

        /**
         * @brief Get memory utilization on Win32
         */
        void GetThreadInfoWin32();

#endif

      private:

        /**
         * Number of cores
         */
        int _numProcessors;

#ifdef __linux__

        /**
         * Last collection timestamp
         */
        clock_t _lastTotalTime = 0;

        /**
         * Last collection timestamp for total CPU utilization
         */
        clock_t _lastTotalCPU = 0;

        /**
         * Last collection timestamp for system CPU utilization
         */
        clock_t _lastTotalSysCPU = 0;

        /**
         * Last collection timestamp for user CPU utilization
         */
        clock_t _lastTotalUserCPU = 0;

        /**
         * Last collection timestamp
         */
        clock_t _lastAwsmockTime = 0;

        /**
         * Last collection timestamp for Awsmock CPU utilization
         */
        unsigned long _lastAwsmockTotalCPU = 0;

        /**
         * Last collection timestamp for system CPU utilization
         */
        unsigned long _lastAwsmockSysCPU = 0;

        /**
         * Last collection timestamp for user CPU utilization
         */
        unsigned long _lastAwsmockUserCPU = 0;

#elif _WIN32

        /**
         * @brief Returns value from WMI
         *
         * @param counter name of the WMI counter
         * @return value of the WMI counter
         */
        static long long GetPerformanceValue(const std::string &counter);

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

#endif