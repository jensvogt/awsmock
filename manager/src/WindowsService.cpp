//
// Created by vogje01 on 27/09/2025.
//

#ifdef _WIN32

// AwsMock includes
#include <awsmock/WindowsService.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/server/Manager.h>
#include <awsmock/service/frontend/FrontendServer.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Manager"};
}

HANDLE g_hEventSource = nullptr;
HANDLE g_ServiceStopEvent = nullptr;
SERVICE_STATUS_HANDLE statusHandle = nullptr;
SERVICE_STATUS serviceStatus = {};

void LogWindowsEvent(const std::string &message, const WORD type = EVENTLOG_ERROR_TYPE) {
    if (const HANDLE hEventLog = RegisterEventSourceA(nullptr, DEFAULT_SERVICE_NAME)) {
        const char *msgs[] = {message.c_str()};
        ReportEventA(hEventLog, type, 0, 0, nullptr, 1, 0, msgs, nullptr);
        DeregisterEventSource(hEventLog);
    }
}

void InstallService(const std::string &exePath, const std::string &serviceName, const std::string &displayName) {

    // Open service manager
    const SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scm) {
        log_error << "OpenSCManager failed";
        return;
    }

    const SC_HANDLE service = CreateService(
        scm,
        serviceName.empty() ? DEFAULT_SERVICE_NAME : serviceName.c_str(),
        displayName.empty() ? DEFAULT_SERVICE_DISPLAY_NAME : displayName.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        exePath.c_str(),
        nullptr, nullptr, nullptr, nullptr, nullptr);

    if (service) {
        log_info << "Service installed";
        SERVICE_DESCRIPTIONA description;
        description.lpDescription = const_cast<LPSTR>(DEFAULT_SERVICE_DESCRIPTION);
        ChangeServiceConfig2A(service, SERVICE_CONFIG_DESCRIPTION, &description);
        CloseServiceHandle(service);
    } else {
        std::cerr << "CreateService failed.\n";
    }

    CloseServiceHandle(scm);
}

void UninstallService(const std::string &serviceName) {

    const SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm) return;

    const SC_HANDLE service = OpenService(scm, serviceName.empty() ? DEFAULT_SERVICE_NAME : serviceName.c_str(), DELETE);
    if (!service) return;

    if (DeleteService(service)) {
        log_info << "Service uninstalled";
    }

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void ReportServiceStatus(const DWORD currentState, const DWORD win32ExitCode, const DWORD waitHint) {
    static DWORD dwCheckPoint = 1;
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = currentState;
    serviceStatus.dwWin32ExitCode = win32ExitCode;
    serviceStatus.dwWaitHint = waitHint;

    if (currentState == SERVICE_RUNNING || currentState == SERVICE_STOPPED) {
        serviceStatus.dwCheckPoint = 0; // reset when stable
    } else {
        serviceStatus.dwCheckPoint = dwCheckPoint++; // increment while pending
    }

    if (currentState == SERVICE_START_PENDING) {
        serviceStatus.dwControlsAccepted = 0;
    } else {
        serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_POWEREVENT | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_SHUTDOWN;
    }
    SetServiceStatus(statusHandle, &serviceStatus);
}

DWORD WINAPI ServiceCtrlHandler(const DWORD CtrlCode, DWORD eventType, LPVOID eventData, LPVOID context) {

    log_info << "Received service control request, code:" << CtrlCode;

    switch (CtrlCode) {
        case SERVICE_CONTROL_STOP:
            // The service is requested to stop.
            log_info << "Received SERVICE_CONTROL_STOP request.";

            // 1. Report pending status.
            ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 4000); // 4 sec wait hint

            // 2. Signal the stop event to the worker thread.
            SetEvent(g_ServiceStopEvent);
            break;

        case SERVICE_CONTROL_INTERROGATE:
            // Interrogate requested. Report current status.
            log_info << "Received SERVICE_CONTROL_INTERROGATE request.";
            ReportServiceStatus(serviceStatus.dwCurrentState, NO_ERROR, 0);
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            // System is shutting down. Treat as a STOP request.
            log_info << "Received SERVICE_CONTROL_SHUTDOWN request.";
            SetEvent(g_ServiceStopEvent);
            break;

        case SERVICE_CONTROL_POWEREVENT:
            // Sleep/wakeup events.
            log_info << "Received SERVICE_CONTROL_POWEREVENT request.";
            break;

        default:
            // Unhandled control request.
            log_warning << "Unhandled control request, code: " << CtrlCode;
            break;
    }
    return 0;
}

DWORD WINAPI RunService(LPVOID lpParam) {

    try {
        
        Awsmock::Service::Frontend::FrontendServer server;
        boost::thread{boost::ref(server), true}.detach();
        log_info << "Frontend server started.";
        boost::asio::io_context ioc;
        Awsmock::Manager::Manager awsMockManager{ioc};

        // Set up endpoints, handlers, and the stop watcher
        awsMockManager.Initialize();
        log_info << "Backend server initialized.";

        // This call will block natively until your stopWatcher calls _ioc.stop()
        awsMockManager.Run(true);
        log_info << "Backend execution loop finished.";

        // Clean up any remaining allocations
        awsMockManager.Stop();
        log_info << "Backend server stopped gracefully.";

    } catch (const std::exception &e) {
        LogWindowsEvent(std::string("RunService exception: ") + e.what());
        log_error << "RunService exception: " << e.what();
        return 1;
    } catch (...) {
        log_error << "RunService unknown exception";
        return 1;
    }
    return 0;
}

void WINAPI ServiceMain(DWORD, LPTSTR *) {

    statusHandle = RegisterServiceCtrlHandlerEx(DEFAULT_SERVICE_NAME, ServiceCtrlHandler, nullptr);
    if (!statusHandle) {
        log_error << "RegisterServiceCtrlHandlerEx failed: " << GetLastError();
        return;
    }

    ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    g_ServiceStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_ServiceStopEvent) {
        ReportServiceStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // No cast needed â€” signature matches exactly
    const HANDLE hThread = CreateThread(nullptr, 0, RunService, nullptr, 0, nullptr);
    if (!hThread) {
        LogWindowsEvent("Failed to create worker thread");
        ReportServiceStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // Block until RunService returns
    WaitForSingleObject(hThread, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);
    if (exitCode != 0) {
        log_error << "Worker thread exited with error code: " << exitCode;
    }

    CloseHandle(hThread);
    CloseHandle(g_ServiceStopEvent);
    ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
}
#endif
