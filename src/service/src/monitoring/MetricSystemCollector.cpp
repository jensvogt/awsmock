//
// Created by vogje01 on 01/05/2023.
//

#include <awsmock/service/monitoring/MetricSystemCollector.h>

namespace AwsMock::Monitoring {

    MetricSystemCollector::MetricSystemCollector() {

        _startTime = system_clock::now();
        _numProcessors = Core::SystemUtils::GetNumberOfCores();
    }

    void MetricSystemCollector::CollectSystemCounter() {
        log_trace << "System collector starting";

#ifdef __APPLE__

        GetThreadInfoMac();
        GetCpuInfoMac();
        GetMemoryInfoMac();

#elif __linux__

        GetCpuInfoAwsmockLinux();
        GetCpuInfoTotalLinux();
        GetMemoryInfoAwsmockLinux();
        GetMemoryInfoTotalLinux();
        GetThreadInfoAwsmockLinux();

#elif _WIN32

        GetCpuInfoWin32();
        GetMemoryInfoWin32();
        GetThreadInfoWin32();

#endif
    }

#ifdef __linux__

    void MetricSystemCollector::GetCpuInfoAwsmockLinux() {

        std::ifstream ifs("/proc/self/stat");
        if (ifs) {
            std::string line;
            std::getline(ifs, line);
            std::vector<std::string> tokens = Core::StringUtils::Split(line, ' ');
            unsigned long userCpu = std::stoul(tokens[13]);
            unsigned long systemCpu = std::stoul(tokens[14]);

            if (_lastAwsmockTotalCPU > 0) {
                double percentUserCpu = static_cast<double>(userCpu - _lastAwsmockUserCPU) / static_cast<double>(userCpu + systemCpu - _lastAwsmockTotalCPU);
                double percentSystemCpu = static_cast<double>(systemCpu - _lastAwsmockSysCPU) / static_cast<double>(userCpu + systemCpu - _lastAwsmockTotalCPU);
                double percentTotalCpu = percentUserCpu + percentSystemCpu;
                MetricService::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "user", percentUserCpu);
                MetricService::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "system", percentSystemCpu);
                MetricService::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "total", percentTotalCpu);
                log_trace << "AwsMock CPU collector, total: " << percentTotalCpu << ", system: " << percentSystemCpu << ", user: " << percentUserCpu;
            }
            _lastAwsmockUserCPU = userCpu;
            _lastAwsmockSysCPU = systemCpu;
            _lastAwsmockTotalCPU = userCpu + systemCpu;
        }
        ifs.close();
    }

    void MetricSystemCollector::GetCpuInfoTotalLinux() {
        tms timeSample{};
        const clock_t now = times(&timeSample);

        if (now - _lastTotalTime > 0) {
            const auto totalPercent = static_cast<double>(timeSample.tms_stime - _lastTotalSysCPU + (timeSample.tms_utime - _lastTotalUserCPU)) / static_cast<double>(now - _lastTotalTime) / _numProcessors * 100;
            MetricService::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "total", totalPercent);
            log_trace << "Total CPU: " << totalPercent;

            const auto userPercent = static_cast<double>(timeSample.tms_utime - _lastTotalUserCPU) / static_cast<double>(now - _lastTotalTime) / _numProcessors * 100;
            MetricService::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "user", userPercent);
            log_trace << "User CPU: " << userPercent;

            const auto systemPercent = static_cast<double>(timeSample.tms_stime - _lastTotalSysCPU) / static_cast<double>(now - _lastTotalTime) / _numProcessors * 100;
            MetricService::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "system", systemPercent);
            log_trace << "System CPU: " << systemPercent;
        }
        _lastTotalTime = now;
        _lastTotalSysCPU = timeSample.tms_stime;
        _lastTotalUserCPU = timeSample.tms_utime;
        log_trace << "System collector finished";
    }

    void MetricSystemCollector::GetMemoryInfoAwsmockLinux() {

        std::ifstream ifs("/proc/self/stat");
        if (std::string line; std::getline(ifs, line)) {
            const std::vector<std::string> tokens = Core::StringUtils::Split(line, ' ');
            MetricService::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "virtual", std::stod(tokens[22]));
            log_trace << "Virtual memory: " << std::stol(tokens[22]);
            MetricService::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "real", std::stod(tokens[23]) * sysconf(_SC_PAGESIZE));
            log_trace << "Real Memory: " << std::stol(tokens[23]);
        }
        ifs.close();
    }

    void MetricSystemCollector::GetMemoryInfoTotalLinux() {

        unsigned long total = 0;
        unsigned long free = 0;

        std::string token;
        std::ifstream file("/proc/meminfo");
        while (file >> token) {
            if (token == "MemTotal:") {
                file >> total;
                total *= 1024;
            }
            if (token == "MemFree:") {
                file >> free;
                free *= 1024;
            }
            // Ignore the rest of the line
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        const double percentUsed = static_cast<double>(total - free) / static_cast<double>(total) * 100;
        MetricService::instance().SetGauge(MEMORY_USAGE_TOTAL, "mem_type", "used", percentUsed);
    }

    void MetricSystemCollector::GetThreadInfoAwsmockLinux() {

        std::ifstream ifs("/proc/self/stat");
        if (std::string line; std::getline(ifs, line)) {
            const std::vector<std::string> tokens = Core::StringUtils::Split(line, ' ');
            MetricService::instance().SetGauge(TOTAL_THREADS, std::stod(tokens[19]));
            log_trace << "Total threads: " << std::stol(tokens[22]);
        }
        ifs.close();
    }

#endif

#ifdef __APPLE__
    void MetricSystemCollector::GetThreadInfoMac() {
        const auto me = mach_task_self();
        thread_array_t threads;
        mach_msg_type_number_t numberOfThreads;

        auto res = task_threads(me, &threads, &numberOfThreads);
        if (res != KERN_SUCCESS) {
            return;
        }

        res = vm_deallocate(me, reinterpret_cast<vm_address_t>(threads), numberOfThreads * sizeof(*threads));
        if (res != KERN_SUCCESS) {
            return;
        }
        MetricService::instance().SetGauge(TOTAL_THREADS, numberOfThreads);
        log_trace << "Total Threads: " << numberOfThreads;
    }

    void MetricSystemCollector::GetCpuInfoMac() {

        rusage r_usage{};

        if (getrusage(RUSAGE_SELF, &r_usage)) {
            log_error << "GetCpuInfoMac failed";
            return;
        }

        if (const long diff = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - _startTime).count(); diff > 0) {

            // User CPU
            long millies = r_usage.ru_utime.tv_sec * 1000 + r_usage.ru_utime.tv_usec;
            double percent = static_cast<double>(millies) / static_cast<double>(diff) * 100;
            MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "user", percent);
            log_trace << "User CPU: " << percent;

            // System CPU
            millies = r_usage.ru_stime.tv_sec * 1000 + r_usage.ru_stime.tv_usec;
            percent = static_cast<double>(millies) / static_cast<double>(diff) * 100;
            MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "system", percent);
            log_trace << "System CPU: " << percent;

            // Total CPU
            millies = r_usage.ru_utime.tv_sec * 1000 + r_usage.ru_utime.tv_usec + r_usage.ru_stime.tv_sec * 1000 + r_usage.ru_stime.tv_usec;
            percent = static_cast<double>(millies) / static_cast<double>(diff) * 100;
            MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "total", percent);
            log_trace << "Total CPU: " << percent;
        }
    }

    void MetricSystemCollector::GetMemoryInfoMac() {

        task_basic_info t_info{};
        mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

        if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&t_info), &t_info_count)) {
            log_error << "Could not get memory utilization";
            return;
        }

        MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "virtual", static_cast<double>(t_info.virtual_size));
        MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "real", static_cast<double>(t_info.resident_size));
        log_trace << "Virtual memory, virtual: " << t_info.virtual_size << " real: " << t_info.resident_size;
    }

#elif _WIN32

    void MetricSystemCollector::GetCpuInfoWin32() {
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "total", GetPerformanceValue("\\Process(awsmockmgr)\\% Processor Time") / _numProcessors);
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "total", GetPerformanceValue("\\Process(awsmockmgr)\\% Privileged Time") / _numProcessors);
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "user", GetPerformanceValue("\\Process(awsmockmgr)\\% User Time") / _numProcessors);
    }

    void MetricSystemCollector::GetMemoryInfoWin32() {
        MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "virtual", GetPerformanceValue("\\Process(awsmockmgr)\\Virtual Bytes"));
        MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "real", GetPerformanceValue("\\Process(awsmockmgr)\\Working Set"));
    }

    void MetricSystemCollector::GetThreadInfoWin32() {
        MetricService::instance().SetGauge(TOTAL_THREADS, GetPerformanceValue("\\Process(awsmockmgr)\\Thread Count"));
    }

    long long MetricSystemCollector::GetPerformanceValue(const std::string &counter) {
        static PDH_STATUS status;
        static PDH_FMT_COUNTERVALUE value;
        static HQUERY query;
        static HCOUNTER counterValue;
        static DWORD ret;

        status = PdhOpenQuery(nullptr, 0, &query);
        if (status != ERROR_SUCCESS) {
            log_error << "PdhOpenQuery error: " << status;
            return 0;
        }

        PdhAddCounter(query, counter.c_str(), 0, &counterValue);
        PdhCollectQueryData(query);

        status = PdhCollectQueryData(query);
        if (status != ERROR_SUCCESS) {
            log_error << "PhdCollectQueryData error: " << status;
            return 0;
        }

        status = PdhGetFormattedCounterValue(counterValue, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &ret, &value);
        if (status != ERROR_SUCCESS) {
            printf("PdhGetFormattedCounterValue() ***Error: 0x%X\n", status);
            return 0;
        }

        return value.doubleValue;
    }
#endif

}// namespace AwsMock::Monitoring