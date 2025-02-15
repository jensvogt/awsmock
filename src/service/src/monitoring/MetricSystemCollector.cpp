//
// Created by vogje01 on 01/05/2023.
//

#include <awsmock/service/monitoring/MetricSystemCollector.h>

namespace AwsMock::Monitoring {
    MetricSystemCollector::MetricSystemCollector() {
        tms timeSample{};

        _lastCPU = times(&timeSample);
        _lastSysCPU = timeSample.tms_stime;
        _lastUserCPU = timeSample.tms_utime;
        _numProcessors = Core::SystemUtils::GetNumberOfCores() / 2;
        _startTime = system_clock::now();
        log_debug << "Got number of processors, numProcs: " << _numProcessors;
    }

    void MetricSystemCollector::CollectSystemCounter() {
        log_trace << "System collector starting";

#ifdef __APPLE__
        GetCurrentThreadCount();
        GetCpuInfo();
#else

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

        tms timeSample{};
        const clock_t now = times(&timeSample);

        log_trace << "Now: " << now << "/" << _lastCPU << "/" << _lastSysCPU << "/" << _lastUserCPU;
        if (now <= _lastCPU || timeSample.tms_stime < _lastSysCPU || timeSample.tms_utime < _lastUserCPU) {
            // Overflow detection. Just skip this value.
            log_debug << "Invalid CPU values, skipping";
        } else {
            double totalPercent = static_cast<double>(timeSample.tms_stime - _lastSysCPU + (timeSample.tms_utime - _lastUserCPU));
            totalPercent /= static_cast<double>(now - _lastCPU);
            totalPercent /= _numProcessors;
            totalPercent *= 100;
            MetricService::instance().SetGauge(TOTAL_CPU, totalPercent);
            log_trace << "Total CPU: " << totalPercent;

            double userPercent = static_cast<double>(timeSample.tms_utime - _lastUserCPU);
            userPercent /= static_cast<double>(now - _lastCPU);
            userPercent /= _numProcessors;
            userPercent *= 100;
            MetricService::instance().SetGauge(USER_CPU, userPercent);
            log_trace << "User CPU: " << userPercent;

            double systemPercent = static_cast<double>(timeSample.tms_stime - _lastSysCPU);
            systemPercent /= static_cast<double>(now - _lastCPU);
            systemPercent /= _numProcessors;
            systemPercent *= 100;
            MetricService::instance().SetGauge(SYSTEM_CPU, systemPercent);
            log_trace << "System CPU: " << systemPercent;
        }
        _lastCPU = now;
        _lastSysCPU = timeSample.tms_stime;
        _lastUserCPU = timeSample.tms_utime;
        log_trace << "System collector finished";
#endif
    }

#ifdef __APPLE__
    void MetricSystemCollector::GetCurrentThreadCount() {
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

    void MetricSystemCollector::GetCpuInfo() {

        rusage r_usage;

        if (getrusage(RUSAGE_SELF, &r_usage)) {
            log_error << "GetCpuInfo failed";
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
#endif

}// namespace AwsMock::Monitoring