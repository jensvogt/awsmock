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
        /*
        boost::asio::io_context ios;
        std::future<std::string> outData, errData;
        boost::process::child c(ios, shellcmd, args, boost::process::std_in.close(), boost::process::std_out > outData, boost::process::std_err > errData);

        // Blocks until command has finished
        ios.run();

        // Get stdout/stderr
        output = outData.get();
        error = errData.get();*/
    }
}// namespace AwsMock::Core