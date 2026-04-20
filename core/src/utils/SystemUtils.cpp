//
// Created by vogje01 on 12/11/2022.
//

#include <awsmock/core/SystemUtils.h>
#include <future>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

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

    std::string SystemUtils::GetRootDir() {
        const boost::filesystem::path path;
        return path.root_directory().string();
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

    int SystemUtils::GetNextFreePort() {
        sockaddr_in sin{};
        constexpr int port = 0;

        const int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == -1)
            return -1;

        sin.sin_port = htons(port);
        sin.sin_addr.s_addr = 0;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_family = AF_INET;

        if (bind(s, reinterpret_cast<sockaddr *>(&sin), sizeof(sockaddr_in)) == -1) {
            if (errno == EADDRINUSE)
                log_error << "Port in use";
            return -1;
        }
        socklen_t len = sizeof(sin);
        if (getsockname(s, reinterpret_cast<sockaddr *>(&sin), &len) != -1) {
#ifdef _WIN32
            closesocket(s);
#else
            shutdown(s, SHUT_RDWR);
            close(s);
#endif
            return ntohs(sin.sin_port);
        }
        return -1;
    }

    int SystemUtils::GetNumberOfCores() {
        return static_cast<int>(boost::thread::hardware_concurrency());
    }

    std::string SystemUtils::GetEnvironmentVariableValue(const std::string &name) {
#ifdef _WIN32
        auto pValue = static_cast<LPTSTR>(malloc(BUFSIZE * sizeof(TCHAR)));
        memset(pValue, 0, BUFSIZE);
        if (const DWORD result = GetEnvironmentVariable(name.c_str(), pValue, BUFSIZE); !result) {
            log_trace << "Environment variable not found, name: " << name << ", error: " << result;
            return {};
        }
        return {pValue};
#else
        if (getenv(name.c_str()) == nullptr) {
            log_debug << "Environment variable not found, name: " << name;
            return {};
        }
        return {getenv(name.c_str())};
#endif
    }

    bool SystemUtils::HasEnvironmentVariable(const std::string &name) {
        return !GetEnvironmentVariableValue(name).empty();
    }

    void SystemUtils::RunShellCommand(const std::string &shellcmd, const std::vector<std::string> &args, std::string &output, std::string &error) {
        log_debug << "Running shell command, cmd: " << shellcmd << ", args: " << StringUtils::Join(args);

        boost::filesystem::path awsExe = boost::process::v2::environment::find_executable("aws");
        log_debug << "Running shell command, cmd: " << awsExe << ", args: " << StringUtils::Join(args);

        boost::asio::io_context ctx;
        boost::asio::readable_pipe outPipe{ctx};
        boost::asio::readable_pipe errPipe{ctx};

#ifdef _WIN32
        boost::process::process proc(ctx, awsExe.string(), args, boost::process::process_stdio{{}, outPipe, errPipe});
#else
        boost::process::process proc(ctx, awsExe, args, boost::process::process_stdio{{}, outPipe, errPipe});
#endif

        boost::system::error_code outEc, errEc;
        boost::asio::async_read(outPipe, boost::asio::dynamic_buffer(output), [&](const boost::system::error_code &ec, std::size_t) { outEc = ec; });
        boost::asio::async_read(errPipe, boost::asio::dynamic_buffer(error), [&](const boost::system::error_code &ec, std::size_t) { errEc = ec; });

        ctx.run();
        proc.wait();

        // Helper: both EOF and broken_pipe are normal pipe-closed signals
        auto isPipeClose = [](const boost::system::error_code &ec) {
            return !ec || ec == boost::asio::error::eof || ec == boost::asio::error::broken_pipe
#ifdef _WIN32
                   || ec == boost::system::error_code(109, boost::system::system_category()) // ERROR_BROKEN_PIPE
#endif
                    ;
        };

        if (!isPipeClose(outEc))
            log_error << "stdout read error: " << outEc.message() << " (" << outEc.value() << ")";
        if (!isPipeClose(errEc))
            log_error << "stderr read error: " << errEc.message() << " (" << errEc.value() << ")";
    }

} // namespace Awsmock::Core

