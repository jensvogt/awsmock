//
// Created by vogje01 on 11/10/2025.
//

#ifndef AWS_MOCK_WINDOWS_SERVICE_H
#define AWS_MOCK_WINDOWS_SERVICE_H

#ifdef _WIN32
void WINAPI ServiceMain(DWORD, LPTSTR *);
void WINAPI InstallService(const std::string &exePath, const std::string &ServiceName, const std::string &displayName);
void WINAPI UninstallService(const std::string &ServiceName);
#define DEFAULT_SERVICE_PATH std::string("C:\\Program Files (x86)\\awsmock\\bin\\awsmockmgr.exe")
#define DEFAULT_SERVICE_NAME "AwsMockService"
#define DEFAULT_SERVICE_DISPLAY_NAME "AWS Mock Service"
#define DEFAULT_SERVICE_DESCRIPTION "Provides AWS simulation service."
#define DEFAULT_CONFIG_FILE std::string("C:\\Program Files (x86)\\awsmock\\etc\\awsmock.json")
#define DEFAULT_SERVICE_PATH std::string("C:\\Program Files (x86)\\awsmock\\bin\\awsmockmgr.exe")
#endif // _WIN32

#endif //AWS_MOCK_WINDOWS_SERVICE_H
