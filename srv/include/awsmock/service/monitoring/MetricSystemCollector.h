//
// Created by vogje01 on 01/05/2023.
//

#pragma once

// C includes
#ifndef _WIN32
#include <sys/times.h>
#endif

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
#include <awsmock/core/EventBus.h>
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>

#ifdef _WIN32
//#include "windows.h"
#include <Wbemidl.h>
#include <comdef.h>
#include <pdh.h>
#endif

namespace Awsmock::Monitoring {

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
         * @brief Destructor, releases the PDH query handle on Windows.
         */
        ~MetricSystemCollector();

        /**
         * @brief Updates the system counter
         */
        void CollectSystemCounter();

#ifdef __APPLE__

        /**
        * @brief Get the number of threads on macOS
        */
        void GetThreadInfoMac();

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
        void GetMemoryInfoMac();

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
        void GetThreadInfoAwsmockLinux() const;

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
        void GetMemoryInfoAwsmockLinux() const;

        /**
         * @brief Get AwsMock memory info on Linux systems
         */
        static void GetMemoryInfoTotalLinux();

#elif _WIN32

        /**
         * @brief Collect all Windows performance counters and emit the corresponding gauges.
         *
         * @par
         * Uses a single, persistent PDH query that is sampled once per collection cycle. Since
         * consecutive cycles are already systemPeriod seconds apart, PDH's rate counters get a
         * valid delta without needing a second, artificially-delayed sample within the call
         * (which used to block the calling thread with Sleep(1000) per counter).
         */
        void CollectSystemCounterWin32() const;

#endif

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Monitoring"};

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
         * @brief Open the PDH query and add all counters once. Called from the constructor.
         */
        void InitPdhQuery();

        /**
         * @brief Read the current formatted value of a previously-added PDH counter.
         *
         * @param counter counter handle, may be null if adding it failed
         * @return counter value, or 0 if the counter is invalid or unreadable
         */
        double ReadCounterValue(HCOUNTER counter) const;

        /**
         * Persistent PDH query, opened once and reused for every collection cycle.
         */
        HQUERY _pdhQuery = nullptr;

        /**
         * Set once the PDH query and all counters have been added successfully.
         */
        bool _pdhInitialized = false;

        HCOUNTER _cpuTotalCounter = nullptr;
        HCOUNTER _cpuSystemCounter = nullptr;
        HCOUNTER _cpuUserCounter = nullptr;
        HCOUNTER _cpuAwsmockTotalCounter = nullptr;
        HCOUNTER _cpuAwsmockSystemCounter = nullptr;
        HCOUNTER _cpuAwsmockUserCounter = nullptr;
        HCOUNTER _memTotalCounter = nullptr;
        HCOUNTER _memAwsmockVirtualCounter = nullptr;
        HCOUNTER _memAwsmockRealCounter = nullptr;
        HCOUNTER _threadCounter = nullptr;

#endif

        /**
         * Start time
         */
        std::chrono::system_clock::time_point _startTime;

        /**
         * Monitoring period
         */
        int _period = 60;
    };

}// namespace Awsmock::Monitoring
