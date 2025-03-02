//
// Created by vogje01 on 01/05/2023.
//

#include <awsmock/service/monitoring/MetricSystemCollector.h>

namespace AwsMock::Monitoring {

    MetricSystemCollector::MetricSystemCollector() {

        _startTime = system_clock::now();

#ifdef _WIN32

        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;

        GetSystemInfo(&sysInfo);
        numProcessors = static_cast<int>(sysInfo.dwNumberOfProcessors);

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        self = GetCurrentProcess();
        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

#endif
    }

    void MetricSystemCollector::CollectSystemCounter() {
        log_trace << "System collector starting";

#ifdef __APPLE__

        GetThreadInfoMac();
        GetCpuInfoMac();
        GetMemoryInfoMac();

#elif __linux__

        GetCpuInfoLinux();
        GetMemoryInfoLinux();
        GetThreadInfoLinux();

#elif _WIN32

        GetCpuInfoWin32();

#endif
    }

#ifdef __linux__

    void MetricSystemCollector::GetCpuInfoLinux() {

        tms timeSample{};
        const clock_t now = times(&timeSample);

        log_trace << "Now: " << now << "/" << _lastTotalCPU << "/" << _lastSysCPU << "/" << _lastUserCPU;
        if (now - _lastTime > 0) {
            const auto totalPercent = static_cast<double>(timeSample.tms_stime - _lastSysCPU + (timeSample.tms_utime - _lastUserCPU)) / static_cast<double>(now - _lastTime) * 100;
            MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "total", totalPercent);
            log_trace << "Total CPU: " << totalPercent;

            const auto userPercent = static_cast<double>(timeSample.tms_utime - _lastUserCPU) / static_cast<double>(now - _lastTime) * 100;
            MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "user", userPercent);
            log_trace << "User CPU: " << userPercent;

            const auto systemPercent = static_cast<double>(timeSample.tms_stime - _lastSysCPU) / static_cast<double>(now - _lastTime) * 100;
            MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "system", systemPercent);
            log_trace << "System CPU: " << systemPercent;
        }
        _lastTime = now;
        _lastTotalCPU = timeSample.tms_stime + timeSample.tms_utime;
        _lastSysCPU = timeSample.tms_stime;
        _lastUserCPU = timeSample.tms_utime;
        log_trace << "System collector finished";
    }

    void MetricSystemCollector::GetMemoryInfoLinux() {

        std::string line;
        std::ifstream ifs("/proc/self/status");
        while (ifs) {
            std::getline(ifs, line);

            if (Core::StringUtils::Contains(line, "VmSize:")) {
                double value = std::stod(Core::StringUtils::Split(line, ':')[1]);
                MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "virtual", value);
                log_trace << "Virtual memory: " << value;
            }
            if (Core::StringUtils::Contains(line, "VmRSS:")) {
                double value = std::stod(Core::StringUtils::Split(line, ':')[1]);
                MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "real", value);
                log_trace << "Real Memory: " << value;
            }
        }
        ifs.close();
    }

    void MetricSystemCollector::GetThreadInfoLinux() {
        std::string line;
        std::ifstream ifs("/proc/self/status");
        while (ifs) {
            std::getline(ifs, line);
            if (Core::StringUtils::Contains(line, "Threads:")) {
                double value = std::stod(Core::StringUtils::Split(line, ':')[1]);
                MetricService::instance().SetGauge(TOTAL_THREADS, value);
                log_trace << "Total Threads: " << value;
            }
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

        if (const long diff = std::chrono::duration_cast<std::chrono::microseconds>(system_clock::now() - _startTime).count(); diff > 0) {

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
        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double percent;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));
        percent /= static_cast<double>(now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "total", percent);
        percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart));
        percent /= static_cast<double>(now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "system", percent);
        percent = static_cast<double>((sys.QuadPart - lastUserCPU.QuadPart));
        percent /= static_cast<double>(now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "user", percent);
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;
    }

    void MetricSystemCollector::GetMemoryInfoWin32() {
        //        PROCESS_MEMORY_COUNTERS_EX pmc;
        //        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *) &pmc, sizeof(pmc));
        //        SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
        //        MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "virtual", static_cast<double>(virtualMemUsedByMe));
    }

#endif

}// namespace AwsMock::Monitoring