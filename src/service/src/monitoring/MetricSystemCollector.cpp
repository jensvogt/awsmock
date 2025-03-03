//
// Created by vogje01 on 01/05/2023.
//

#include <awsmock/service/monitoring/MetricSystemCollector.h>

#ifdef _WIN32
#include <psapi.h>

extern "C" {
long getVirtualMemory() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc));
    return static_cast<long>(pmc.PrivateUsage);
}

long getRealMemory() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc));
    return static_cast<long>(pmc.WorkingSetSize);
}
}
#endif

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

        HRESULT hres;


        // Step 1: --------------------------------------------------
        // Initialize COM. ------------------------------------------

        hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hres)) {
            log_error << "Failed to initialize COM library. Error code = 0x" << std::hex << hres;
            return;
        }

        // Step 2: --------------------------------------------------
        // Set general COM security levels --------------------------

        hres = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
        if (FAILED(hres)) {
            log_error << "Failed to initialize security. Error code = 0x" << std::hex;
            CoUninitialize();
            return;
        }

        // Step 3: ---------------------------------------------------
        // Obtain the initial locator to WMI -------------------------

        IWbemLocator *pLoc = nullptr;
        hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLoc);
        if (FAILED(hres)) {
            log_error << "Failed to create IWbemLocator object." << " Err code = 0x" << std::hex << hres;
            CoUninitialize();
            return;
        }

        // Step 4: -----------------------------------------------------
        // Connect to WMI through the IWbemLocator::ConnectServer method


        // Connect to the root\cimv2 namespace with the current user and obtain pointer pSvc to make IWbemServices calls.
        hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0, NULL, 0, 0, &pSvc);
        if (FAILED(hres)) {
            log_error << "Could not connect. Error code = 0x" << std::hex << hres;
            pLoc->Release();
            CoUninitialize();
            return;
        }

        // Step 5: --------------------------------------------------
        // Set security levels on the proxy -------------------------

        hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        if (FAILED(hres)) {
            log_error << "Could not set proxy blanket. Error code = 0x" << std::hex << hres;
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return;
        }
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
        GetRealMemoryInfoWin32();
        GetVirtualMemoryInfoWin32();
        GetThreadInfoWin32();

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

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        double percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart)) / (static_cast<double>(now.QuadPart - lastCPU.QuadPart) * numProcessors);
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "total", percent);
        percent = static_cast<double>(sys.QuadPart - lastSysCPU.QuadPart) / (static_cast<double>(now.QuadPart - lastCPU.QuadPart) * numProcessors);
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "system", percent);
        percent = static_cast<double>(sys.QuadPart - lastUserCPU.QuadPart) / (static_cast<double>(now.QuadPart - lastCPU.QuadPart) * numProcessors);
        MetricService::instance().SetGauge(CPU_USAGE, "cpu_type", "user", percent);
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;
    }

    void MetricSystemCollector::GetRealMemoryInfoWin32() {

        IEnumWbemClassObject *pEnumerator = nullptr;
        HRESULT hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT *  FROM Win32_Process WHERE Name = 'awsmockmgr.exe'"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
        if (FAILED(hres)) {
            log_error << "Query for operating system name failed. Error code = 0x" << std::hex << hres;
            pSvc->Release();
            //pLoc->Release();
            CoUninitialize();
            return;
        }

        IWbemClassObject *pclsObj = nullptr;
        ULONG uReturn = 0;
        while (pEnumerator) {
            pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (0 == uReturn) {
                break;
            }

            VARIANT vtProp;
            VariantInit(&vtProp);

            // Get the value of working set (real memory)
            pclsObj->Get(L"WorkingSetSize", 0, &vtProp, 0, 0);
            long long realMem = std::stoll(vtProp.bstrVal);
            log_info << "WS: " << realMem;
            MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "real", realMem);

            // Cleanup
            VariantClear(&vtProp);
            pclsObj->Release();
        }

        // Cleanup
        pEnumerator->Release();
    }

    void MetricSystemCollector::GetVirtualMemoryInfoWin32() {

        IEnumWbemClassObject *pEnumerator = nullptr;
        HRESULT hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_Process WHERE Name = 'awsmockmgr.exe'"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
        if (FAILED(hres)) {
            log_error << "Query for operating system name failed. Error code = 0x" << std::hex << hres;
            pSvc->Release();
            CoUninitialize();
            return;
        }

        IWbemClassObject *pclsObj = nullptr;
        ULONG uReturn = 0;
        while (pEnumerator) {
            pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (0 == uReturn) {
                break;
            }

            VARIANT vtProp;
            VariantInit(&vtProp);

            // Get the value of working set (real memory)
            hres = pclsObj->Get(L"VirtualSize", 0, &vtProp, 0, 0);
            long long virtMem = std::stoll(vtProp.bstrVal);
            log_info << "VM: " << virtMem;
            MetricService::instance().SetGauge(MEMORY_USAGE, "mem_type", "virtual", virtMem);

            // Cleanup
            VariantClear(&vtProp);
            pclsObj->Release();
        }

        // Cleanup
        pEnumerator->Release();
    }

    void MetricSystemCollector::GetThreadInfoWin32() {

        // For example, get the name of the operating system
        IEnumWbemClassObject *pEnumerator = nullptr;
        HRESULT hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_Process WHERE Name = 'awsmockmgr.exe'"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
        if (FAILED(hres)) {
            log_error << "Query for operating system name failed. Error code = 0x" << std::hex << hres;
            pSvc->Release();
            CoUninitialize();
            return;
        }

        IWbemClassObject *pclsObj = nullptr;
        ULONG uReturn = 0;
        while (pEnumerator) {
            pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (0 == uReturn) {
                break;
            }

            VARIANT vtProp;
            VariantInit(&vtProp);

            // Get the value of the Name property
            pclsObj->Get(L"ThreadCount", 0, &vtProp, 0, 0);
            log_info << "Thread Count: " << vtProp.lVal;
            MetricService::instance().SetGauge(TOTAL_THREADS, vtProp.lVal);
            VariantClear(&vtProp);

            pclsObj->Release();
        }

        // Cleanup
        pEnumerator->Release();
    }
#endif

}// namespace AwsMock::Monitoring