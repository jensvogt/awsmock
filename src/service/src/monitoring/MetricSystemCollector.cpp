//
// Created by vogje01 on 01/05/2023.
//

#include <awsmock/service/monitoring/MetricSystemCollector.h>

namespace AwsMock::Monitoring {

    MetricSystemCollector::MetricSystemCollector() {
        _startTime = system_clock::now();
    }

    void MetricSystemCollector::CollectSystemCounter() {
        log_trace << "System collector starting";

#ifdef __APPLE__

        GetCurrentThreadCountMac();
        GetCpuInfoMac();
        GetMemoryInfoMac();

#elif __linux__

        GetCpuInfoLinux();
        GetMemoryThreadsInfoLinux();

#endif
    }

#ifdef __linux__

    void MetricSystemCollector::GetCpuInfoLinux() {

        // Get uptime
        struct sysinfo s_info {};
        if (sysinfo(&s_info) != 0) {
            log_error << "Could not get sysinfo";
            return;
        }

        // Get CPU utilization
        tms timeSample{};
        times(&timeSample);

        if (s_info.uptime > 0) {
            double percent = static_cast<double>(timeSample.tms_utime) / static_cast<double>(s_info.uptime) * 100;
            MetricService::instance().SetGauge(USER_CPU, percent);
            log_trace << "User CPU: " << percent;

            percent = static_cast<double>(timeSample.tms_stime) / static_cast<double>(s_info.uptime) * 100;
            MetricService::instance().SetGauge(SYSTEM_CPU, percent);
            log_trace << "System CPU: " << percent;

            percent = static_cast<double>(timeSample.tms_utime + timeSample.tms_stime) / static_cast<double>(s_info.uptime) * 100;
            MetricService::instance().SetGauge(TOTAL_CPU, percent);
            log_trace << "Total CPU: " << percent;
        }
        log_trace << "System collector finished";
    }

    void MetricSystemCollector::GetMemoryThreadsInfoLinux() {

        std::string line;
        std::ifstream ifs("/proc/self/status");
        while (ifs) {
            std::getline(ifs, line);

            if (Core::StringUtils::Contains(line, "VmSize:")) {
                double value = std::stod(Core::StringUtils::Split(line, ':')[1]);
                MetricService::instance().SetGauge(VIRTUAL_MEMORY, value);
                log_trace << "Virtual memory: " << value;
            }
            if (Core::StringUtils::Contains(line, "VmRSS:")) {
                double value = std::stod(Core::StringUtils::Split(line, ':')[1]);
                MetricService::instance().SetGauge(REAL_MEMORY, value);
                log_trace << "Real Memory: " << value;
            }
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
    void MetricSystemCollector::GetCurrentThreadCountMac() {
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

        rusage r_usage;

        if (getrusage(RUSAGE_SELF, &r_usage)) {
            log_error << "GetCpuInfoMac failed";
            return;
        }

        if (const long diff = std::chrono::duration_cast<std::chrono::microseconds>(system_clock::now() - _startTime).count(); diff > 0) {

            // User CPU
            long millies = r_usage.ru_utime.tv_sec * 1000 + r_usage.ru_utime.tv_usec;
            double percent = static_cast<double>(millies) / static_cast<double>(diff) * 100;
            MetricService::instance().SetGauge(USER_CPU, percent);
            log_trace << "User CPU: " << percent;

            // System CPU
            millies = r_usage.ru_stime.tv_sec * 1000 + r_usage.ru_stime.tv_usec;
            percent = static_cast<double>(millies) / static_cast<double>(diff) * 100;
            MetricService::instance().SetGauge(SYSTEM_CPU, percent);
            log_trace << "System CPU: " << percent;

            // Total CPU
            millies = r_usage.ru_utime.tv_sec * 1000 + r_usage.ru_utime.tv_usec + r_usage.ru_stime.tv_sec * 1000 + r_usage.ru_stime.tv_usec;
            percent = static_cast<double>(millies) / static_cast<double>(diff) * 100;
            MetricService::instance().SetGauge(TOTAL_CPU, percent);
            log_trace << "Total CPU: " << percent;
        }
    }

    void MetricSystemCollector::GetMemoryInfoMac() {

        struct task_basic_info t_info;
        mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

        if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t) &t_info, &t_info_count)) {
            log_error << "Could not get memory utilization";
            return;
        }

        MetricService::instance().SetGauge(VIRTUAL_MEMORY, t_info.virtual_size);
        MetricService::instance().SetGauge(REAL_MEMORY, t_info.resident_size);
        log_trace << "Virtual memory, virtual: " << t_info.virtual_size << " real: " << t_info.resident_size;
    }

#endif

}// namespace AwsMock::Monitoring