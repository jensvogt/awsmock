//
// Created by vogje01 on 12/11/2022.
//

#include <awsmock/core/SystemUtils.h>
#include <boost/asio/readable_pipe.hpp>

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
        return static_cast<int>(boost::thread::hardware_concurrency());
    }

    std::string SystemUtils::GetEnvironmentVariableValue(const std::string &name) {
        char *pValue;
#ifdef _WIN32
        pValue = static_cast<LPTSTR>(malloc(BUFSIZE * sizeof(TCHAR)));
        if (DWORD result; (result = GetEnvironmentVariable(name.c_str(), pValue, BUFSIZE))) {
            log_info << "Environment variable not found, name: " << name << ", error: " << result;
        }
        return {pValue};
#else
        size_t len;
        if (const errno_t result = _dupenv_s(&pValue, &len, name.c_str())) {
            log_info << "Environment variable not found, name: " << name << ", error: " << result;
        }
        return {pValue, len};
#endif
    }

    bool SystemUtils::HasEnvironmentVariable(const std::string &name) {
        return !GetEnvironmentVariableValue(name).empty();
    }

    void SystemUtils::RunShellCommand(const std::string &shellcmd, const std::vector<std::string> &args, const std::string &input, std::string &output, std::string &error) {

        boost::asio::io_context ctx;
        boost::asio::readable_pipe inPipe{ctx};
        boost::asio::readable_pipe outPipe{ctx};
        boost::asio::readable_pipe errPipe{ctx};
        boost::process::process proc(ctx, shellcmd, args, boost::process::process_stdio{inPipe, outPipe, errPipe});
        boost::system::error_code ec;
        boost::asio::read(outPipe, boost::asio::dynamic_buffer(output), ec);
        assert(!ec || (ec == boost::asio::error::eof));
        boost::asio::read(errPipe, boost::asio::dynamic_buffer(error), ec);
        assert(!ec || (ec == boost::asio::error::eof));
        proc.wait();
    }
}// namespace AwsMock::Core