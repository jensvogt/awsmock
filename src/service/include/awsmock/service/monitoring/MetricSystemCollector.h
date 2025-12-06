//
// Created by vogje01 on 01/05/2023.
//

#ifndef AWSMOCK_MONITORING_METRIC_SYSTEM_COLLECTOR_H
#define AWSMOCK_MONITORING_METRIC_SYSTEM_COLLECTOR_H

// C includes
#ifndef _WIN32
#include <sys/times.h>
#endif
#include "../../../../../core/include/awsmock/core/monitoring/MonitoringCollector.h"

#ifdef __linux__
#include <sys/sysinfo.h>
#elif __APPLE__
#include <mach/mach.h>
#include <sys/resource.h>
#define TO_MICROS 1000000
#endif

// C++ Standard includes
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

#ifdef _WIN32
#include "windows.h"
#include <Wbemidl.h>
#include <comdef.h>
#include <pdh.h>
#endif

namespace AwsMock::Monitoring {

    using std::chrono::microseconds;
#ifdef __linux__
    struct CpuTimes {
        unsigned long long user = 0;
        unsigned long long nice = 0;
        unsigned long long system = 0;
        unsigned long long idle = 0;
        unsigned long long iowait = 0;
        unsigned long long irq = 0;
        unsigned long long softirq = 0;
        unsigned long long steal = 0;
        unsigned long long total = 0;
        bool initialized = false;
    };

    struct CpuProcTimes {
        unsigned long long utime = 0;
        unsigned long long stime = 0;
        bool initialized = false;
    };

#endif

    /**
     * @brief Collect system information like CPU and Memory.
     *
     * Runs as a background thread with a given timeout in ms.
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
        * @brief Get the number of threads on macOS
        */
        static void GetThreadInfoMac();

        /**
         * @brief Get CPU utilization on macOS
         */
        void GetCpuInfoAwsmockMac() const;

        /**
         * @brief Get CPU utilization on macOS
         */
        void GetCpuInfoTotalMac();

        /**
         * @brief Get memory utilization on macOS
         */
        static void GetMemoryInfoMac();

#elif __linux__

        /**
         * @brief Returns the total CPU counter
         *
         * @return total CPU counter
         */
        static CpuTimes ReadCpuTimes();

        /**
         * @brief Returns the AwsMock CPU counter
         *
         * @return AwsMock CPU counter
         */
        static CpuProcTimes ReadProcCpuTimes();

        /**
         * @brief Get the number of threads on macOS
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
        static void GetCpuInfoWin32() ;

        /**
         * @brief Get CPU utilization on Windows
         */
        static void GetCpuInfoAwsmockWin32() ;

        /**
         * @brief Get total memory utilization on Win32
         */
        static void GetMemoryInfoWin32() ;

        /**
         * @brief Get memory utilization on Win32
         */
        static void GetMemoryInfoAwsmockWin32();

        /**
         * @brief Get memory utilization on Win32
         */
        static void GetThreadInfoWin32();

#endif

      private:

        /**
         * Number of cores
         */
        int _numProcessors;

#ifdef __APPLE__

        /**
         * @brief Calculate the total CPU
         *
         * @par
         * On macOS only the total CPU usage can be retrieved from the system, User and system usage are not available.
         */
        void CalculateCPULoadMac(unsigned long long idleTicks, unsigned long long totalTicks);
        unsigned long long _previousTotalTicks;
        unsigned long long _previousIdleTicks;

#elif __linux__

        /**
         * Previous total CPU counter
         */
        CpuTimes _previousCpuTimes;

        /**
         * Current total CPU counter
         */
        CpuTimes _currentCpuTimes;

        /**
         * Previous total CPU counter
         */
        CpuTimes _previousProcCpuTimes;

        /**
         * Current total CPU counter
         */
        CpuTimes _currentProcCpuTimes;

        /**
         * Previous process CPU counter
         */
        CpuProcTimes _previousProcCpuTime;

        /**
         * Current process CPU counter
         */
        CpuProcTimes _currentProcCpuTime;

#elif _WIN32

        /**
         * @brief Returns value from WMI
         *
         * @param counter name of the WMI counter
         * @return value of the WMI counter
         */
        static double GetPerformanceValue(const std::string &counter);

#endif

        /**
         * Start time
         */
        system_clock::time_point _startTime;

        /**
         * Monitoring period
         */
        int _period = 60;

        /**
         * Map of monitoring counters
         */
        Core::MonitoringCollector &_shmUtils;
    };

}// namespace AwsMock::Monitoring

#endif