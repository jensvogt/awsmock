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
#include <awsmock/core/Macros.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

#ifdef _WIN32
#include "windows.h"
#include <Wbemidl.h>
#include <comdef.h>
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
        explicit AWSMOCK_API MetricSystemCollector();

        /**
         * @brief Updates the system counter
         */
        AWSMOCK_API void CollectSystemCounter();

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
        void GetCpuInfoWin32() const;

        /**
         * @brief Get memory utilization on Win32
         */
        void GetMemoryInfoWin32() const;

        /**
         * @brief Get memory utilization on Win32
         */
        void GetThreadInfoWin32() const;

#endif

      private:

#ifdef __linux__
        clock_t _lastTime = 0;
        clock_t _lastTotalCPU = 0;
        clock_t _lastSysCPU = 0;
        clock_t _lastUserCPU = 0;
#elif _WIN32

        /**
         * Windows WMI service
         */
        IWbemServices *pSvc = nullptr;

        /**
         * @brief Returns value from WMI
         *
         * @param counter name of the WMI counter
         * @return value of the WMI counter
         */
        long long GetPerformanceValue(const std::string &counter) const;

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
