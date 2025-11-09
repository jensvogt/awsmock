//
// Created by vogje01 on 01/05/2023.
//

#include <awsmock/service/monitoring/MetricSystemCollector.h>

namespace AwsMock::Monitoring {

    MetricSystemCollector::MetricSystemCollector() : _shmUtils(Core::MonitoringCollector::instance()) {

        _startTime = system_clock::now();
        _numProcessors = Core::SystemUtils::GetNumberOfCores();
    }

    void MetricSystemCollector::CollectSystemCounter() {
        log_trace << "System collector starting";

#ifdef __APPLE__

        GetThreadInfoMac();
        GetCpuInfoAwsmockMac();
        GetCpuInfoTotalMac();
        GetMemoryInfoMac();

#elif __linux__

        GetCpuInfoAwsmockLinux();
        GetCpuInfoTotalLinux();
        GetMemoryInfoAwsmockLinux();
        GetMemoryInfoTotalLinux();
        GetThreadInfoAwsmockLinux();

#elif _WIN32

        GetCpuInfoWin32();
        GetCpuInfoAwsmockWin32();
        GetMemoryInfoWin32();
        GetMemoryInfoAwsmockWin32();
        GetThreadInfoWin32();

#endif
    }

#ifdef __linux__

    void MetricSystemCollector::GetCpuInfoAwsmockLinux() {
        const auto nproc = static_cast<double>(sysconf(_SC_NPROCESSORS_ONLN));

        // first snapshot
        _currentProcCpuTime = ReadProcCpuTimes();
        _currentProcCpuTimes = ReadCpuTimes();

        if (_previousProcCpuTime.initialized) {

            // second snapshot
            _currentProcCpuTime = ReadProcCpuTimes();
            _currentProcCpuTimes = ReadCpuTimes();

            const auto utimeDiff = static_cast<double>(_currentProcCpuTime.utime - _previousProcCpuTime.utime);
            const auto stimeDiff = static_cast<double>(_currentProcCpuTime.stime - _previousProcCpuTime.stime);
            const auto sysDiff = static_cast<double>(_currentProcCpuTimes.total - _previousProcCpuTimes.total);
            const auto procDiff = utimeDiff + stimeDiff;

            if (sysDiff > 0 && nproc > 0) {
                if (const auto userPercent = 100.0 * utimeDiff / sysDiff / nproc; std::isnormal(userPercent)) {
                    Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "user", userPercent);
                }
                if (const auto systemPercent = 100.0 * stimeDiff / sysDiff / nproc; std::isnormal(systemPercent)) {
                    Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "system", systemPercent);
                }
                if (const auto totalPercent = 100.0 * procDiff / sysDiff / nproc; std::isnormal(totalPercent)) {
                    Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "total", totalPercent);
                }
            }
        }
        _previousProcCpuTimes = _currentProcCpuTimes;
        _previousProcCpuTime = _currentProcCpuTime;
    }

    CpuTimes MetricSystemCollector::ReadCpuTimes() {
        std::ifstream file("/proc/stat");
        CpuTimes cpu;

        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (line.rfind("cpu ", 0) == 0) {// first line "cpu ..."
                    std::istringstream ss(line);
                    std::string cpuLabel;
                    ss >> cpuLabel >> cpu.user >> cpu.nice >> cpu.system >> cpu.idle >> cpu.iowait >> cpu.irq >> cpu.softirq >> cpu.steal;
                    cpu.total = cpu.user + cpu.nice + cpu.system + cpu.idle + cpu.iowait + cpu.irq + cpu.softirq - cpu.steal;
                    cpu.initialized = true;
                    break;
                }
            }
        }
        file.close();
        return cpu;
    }

    CpuProcTimes MetricSystemCollector::ReadProcCpuTimes() {
        std::ifstream file("/proc/self/stat");
        std::string line;
        CpuProcTimes pt;
        if (!file.is_open()) return pt;

        std::getline(file, line);
        std::istringstream ss(line);
        std::string token;

        // skip first 13 fields
        for (int i = 0; i < 13; i++) ss >> token;

        ss >> pt.utime >> pt.stime;// fields 14 and 15
        pt.initialized = true;
        return pt;
    }

    void MetricSystemCollector::GetCpuInfoTotalLinux() {

        _currentCpuTimes = ReadCpuTimes();
        if (_previousCpuTimes.initialized) {

            const auto userDiff = static_cast<double>(_currentCpuTimes.user + _currentCpuTimes.nice - (_previousCpuTimes.user + _previousCpuTimes.nice));
            const auto systemDiff = static_cast<double>(_currentCpuTimes.system + _currentCpuTimes.irq + _currentCpuTimes.softirq - (_previousCpuTimes.system + _previousCpuTimes.irq + _previousCpuTimes.softirq));
            const auto idleDiff = static_cast<double>(_currentCpuTimes.idle + _currentCpuTimes.iowait - (_previousCpuTimes.idle + _previousCpuTimes.iowait));

            const auto totalDiff = userDiff + systemDiff + idleDiff + static_cast<double>(_currentCpuTimes.steal - _previousCpuTimes.steal);
            if (const auto totalPercent = 100.0 - 100.0 * idleDiff / totalDiff; std::isnormal(totalPercent)) {
                Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "total", totalPercent);
            }
            if (const auto systemPercent = 100.0 * systemDiff / totalDiff; std::isnormal(systemPercent)) {
                Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "system", systemPercent);
            }
            if (const auto userPercent = 100.0 * userDiff / totalDiff; std::isnormal(userPercent)) {
                Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "user", userPercent);
            }
        }
        _previousCpuTimes = _currentCpuTimes;
    }

    void MetricSystemCollector::GetMemoryInfoAwsmockLinux() {

        std::ifstream ifs("/proc/self/stat");
        if (std::string line; std::getline(ifs, line)) {
            const std::vector<std::string> tokens = Core::StringUtils::Split(line, " ");
            Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "virtual", std::stod(tokens[22]));
            log_trace << "Virtual memory: " << std::stol(tokens[22]);
            Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "real", std::stod(tokens[23]) * static_cast<double>(sysconf(_SC_PAGESIZE)));
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
        if (const double percentUsed = static_cast<double>(total - free) / static_cast<double>(total) * 100; std::isnormal(percentUsed)) {
            Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_TOTAL, "mem_type", "used", percentUsed);
        }
    }

    void MetricSystemCollector::GetThreadInfoAwsmockLinux() {

        std::ifstream ifs("/proc/self/stat");
        if (std::string line; std::getline(ifs, line)) {
            const std::vector<std::string> tokens = Core::StringUtils::Split(line, " ");
            Core::MonitoringCollector::instance().SetGauge(TOTAL_THREADS, Core::NumberUtils::ToDouble(tokens[19]));
            log_debug << "Total threads: " << std::stod(tokens[19]);
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
        Core::MonitoringCollector::instance().SetGauge(TOTAL_THREADS, static_cast<double>(numberOfThreads));
        log_trace << "Total Threads: " << numberOfThreads;
    }

    void MetricSystemCollector::GetCpuInfoAwsmockMac() const {

        rusage r_usage{};

        if (getrusage(RUSAGE_SELF, &r_usage)) {
            log_error << "GetCpuInfoMac failed";
            return;
        }

        if (const long diff = std::chrono::duration_cast<microseconds>(system_clock::now() - _startTime).count(); diff > 0) {

            // User CPU
            long micros = r_usage.ru_utime.tv_sec * TO_MICROS + r_usage.ru_utime.tv_usec;
            double percent = static_cast<double>(micros) / static_cast<double>(diff) * 100;
            Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "user", percent);
            log_trace << "User CPU: " << percent;

            // System CPU
            micros = r_usage.ru_stime.tv_sec * TO_MICROS + r_usage.ru_stime.tv_usec;
            percent = static_cast<double>(micros) / static_cast<double>(diff) * 100;
            Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "system", percent);
            log_trace << "System CPU: " << percent;

            // Total CPU
            micros = r_usage.ru_utime.tv_sec * TO_MICROS + r_usage.ru_utime.tv_usec + r_usage.ru_stime.tv_sec * TO_MICROS + r_usage.ru_stime.tv_usec;
            percent = static_cast<double>(micros) / static_cast<double>(diff) * 100;
            Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "total", percent);
            log_trace << "Total CPU: " << percent;
        }
    }

    void MetricSystemCollector::GetCpuInfoTotalMac() {

        host_cpu_load_info_data_t cpuinfo;
        mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
        if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, reinterpret_cast<host_info_t>(&cpuinfo), &count) == KERN_SUCCESS) {
            unsigned long long totalTicks = 0;
            for (const unsigned int cpu_tick: cpuinfo.cpu_ticks) {
                totalTicks += cpu_tick;
            }
            CalculateCPULoadMac(cpuinfo.cpu_ticks[CPU_STATE_IDLE], totalTicks);
        }
    }

    void MetricSystemCollector::CalculateCPULoadMac(const unsigned long long idleTicks, const unsigned long long totalTicks) {
        const unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
        const unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;
        const float totalPercent = 1.0f - (totalTicksSinceLastTime > 0 ? static_cast<float>(idleTicksSinceLastTime) / static_cast<float>(totalTicksSinceLastTime) : 0);
        Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "total", totalPercent);
        _previousTotalTicks = totalTicks;
        _previousIdleTicks = idleTicks;
    }

    void MetricSystemCollector::GetMemoryInfoMac() {

        task_basic_info t_info{};
        mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

        if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&t_info), &t_info_count)) {
            log_error << "Could not get memory utilization";
            return;
        }

        Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "virtual", static_cast<double>(t_info.virtual_size));
        Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "real", static_cast<double>(t_info.resident_size));
        log_trace << "Virtual memory, virtual: " << t_info.virtual_size << " real: " << t_info.resident_size;
    }

#elif _WIN32

    void MetricSystemCollector::GetCpuInfoWin32()  {
        Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "total", GetPerformanceValue("\\Processor Information(_Total)\\% Processor Time"));
        Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "system",  GetPerformanceValue("\\Processor Information(_Total)\\% Privileged Time"));
        Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_TOTAL, "cpu_type", "user",  GetPerformanceValue("\\Processor Information(_Total)\\% User Time"));
    }

    void MetricSystemCollector::GetCpuInfoAwsmockWin32()  {
        Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "total", GetPerformanceValue("\\Process(awsmockmgr)\\% Processor Time"));
        Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "system", GetPerformanceValue("\\Process(awsmockmgr)\\% Privileged Time"));
        Core::MonitoringCollector::instance().SetGauge(CPU_USAGE_AWSMOCK, "cpu_type", "user", GetPerformanceValue("\\Process(awsmockmgr)\\% User Time"));
    }

    void MetricSystemCollector::GetMemoryInfoWin32()  {
        Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_TOTAL, "mem_type", "used", static_cast<double>(GetPerformanceValue("\\Memory\\% Committed Bytes In Use")));
    }

    void MetricSystemCollector::GetMemoryInfoAwsmockWin32() {
        Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "virtual", static_cast<double>(GetPerformanceValue("\\Process(awsmockmgr)\\Virtual Bytes")));
        Core::MonitoringCollector::instance().SetGauge(MEMORY_USAGE_AWSMOCK, "mem_type", "real", static_cast<double>(GetPerformanceValue("\\Process(awsmockmgr)\\Working Set")));
    }

    void MetricSystemCollector::GetThreadInfoWin32() {
        Core::MonitoringCollector::instance().SetGauge(TOTAL_THREADS, {}, {}, static_cast<double>(GetPerformanceValue("\\Process(awsmockmgr)\\Thread Count")));
    }

    double MetricSystemCollector::GetPerformanceValue(const std::string &counter) {
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

        status = PdhAddCounter(query, counter.c_str(), 0, &counterValue);
        if (status != ERROR_SUCCESS) {
            log_error << "PdhAddCounter error: " << status;
            return 0;
        }

        status = PdhCollectQueryData(query);
        if (status != ERROR_SUCCESS) {
            log_error << "PdhCollectQueryData error: " << status;
            return 0;
        }
        Sleep(1000);

        status = PdhCollectQueryData(query);
        if (status != ERROR_SUCCESS) {
            log_error << "PhdCollectQueryData error: " << status;
            return 0;
        }

        status = PdhGetFormattedCounterValue(counterValue, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &ret, &value);
        if (status != ERROR_SUCCESS) {
            log_error << "PdhGetFormattedCounterValue, error: " << static_cast<int>(status);
            return 0;
        }

        // Clean up the query handle
        PdhCloseQuery(query);

        return value.doubleValue;
    }
#endif

}// namespace AwsMock::Monitoring