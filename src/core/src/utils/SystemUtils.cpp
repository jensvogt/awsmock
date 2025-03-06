//
// Created by vogje01 on 12/11/2022.
//

#include <awsmock/core/SystemUtils.h>

namespace AwsMock::Core {

    std::string SystemUtils::GetCurrentWorkingDir() {
        return boost::filesystem::current_path().string();
    }

    std::string SystemUtils::GetHomeDir() {
        std::string homeDir;
#ifdef WIN32
        size_t size = 1024;
        auto buffer = static_cast<char *>(malloc(size));
        _dupenv_s(&buffer, &size, "HOMEPATH");
        if (buffer != nullptr) {
            homeDir = std::string(buffer);
        }
        free(buffer);
#else
        if (getenv("HOME") != nullptr) {
            homeDir = std::string(getenv("HOME"));
        } else {
            homeDir = std::string(getpwuid(getuid())->pw_dir);
        }
#endif
        return homeDir;
    }

    std::string SystemUtils::GetHostName() {
        return boost::asio::ip::host_name();
    }

    int SystemUtils::GetRandomPort() {
        return RandomUtils::NextInt(RANDOM_PORT_MIN, RANDOM_PORT_MAX);
    }

    int SystemUtils::GetPid() {
#ifdef WIN32
        return _getpid();
#else
        return getpid();
#endif
    }

    int SystemUtils::GetNumberOfCores() {
#ifdef __APPLE__
        // TODO: Check macOS
        int count;
        size_t countLen;
        sysctlbyname("hw.logicalcpu", &count, &countLen, nullptr, 0);
        return count;
#else
        return static_cast<int>(boost::thread::hardware_concurrency());
#endif
    }

    void SystemUtils::RunShellCommand(const std::string &shellcmd, const std::vector<std::string> &args, const std::string &input, std::string &output, std::string &error) {

        boost::asio::io_context ios;
        std::future<std::string> outData, errData;
        boost::process::child c(ios, shellcmd, args, boost::process::std_in.close(), boost::process::std_out > outData, boost::process::std_err > errData);

        // Blocks until command has finished
        ios.run();

        // Get stdout/stderr
        output = outData.get();
        error = errData.get();
    }
}// namespace AwsMock::Core