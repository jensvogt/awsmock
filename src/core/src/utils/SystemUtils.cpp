//
// Created by vogje01 on 12/11/2022.
//

#include <awsmock/core/SystemUtils.h>

namespace AwsMock::Core {

    ExecResult SystemUtils::Exec(const std::string &command) {

        // set up file redirection
        std::filesystem::path redirection = std::filesystem::absolute(".output.temp");
        std::string cmd = command + " > " + redirection.string() + " 2>&1";

        // execute command
#if __APPLE__
        auto status = std::system(cmd.c_str());
#elif _WIN32
        // TODO: Windows port
        auto status = 1;
#else
        auto status = WEXITSTATUS(std::system(cmd.c_str()));
#endif
        log_trace << "Exec status: " << status;

        // read redirection file and remove the file
        std::ifstream output_file(redirection);
        std::string output((std::istreambuf_iterator(output_file)), std::istreambuf_iterator<char>());
        std::filesystem::remove(redirection);

        return ExecResult{status - 120, output};
    }

    ExecResult SystemUtils::Exec2(const std::string &command) {
        std::array<char, 128> buffer{};
        std::string result;
#ifdef WIN32
        // TODO: Windows port
        return ExecResult{0, result};
#else
        const std::unique_ptr<FILE, void (*)(FILE *)> pipe(popen(command.c_str(), "r"),
                                                           [](FILE *f) -> void {
                                                               std::ignore = pclose(f);
                                                           });
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return ExecResult{0, result};
#endif
    }

    std::string SystemUtils::GetCurrentWorkingDir() {
#ifdef WIN32
        // TODO: Windows port
        return "";
#else
        char result[PATH_MAX];
        const ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        const std::string path = {std::string(result, (count > 0) ? count : 0)};
        return path.substr(0, path.find_last_of('/'));
#endif
    }

    std::string SystemUtils::GetHomeDir() {
        std::string homeDir;
#ifdef WIN32
        // TODO: Windows port
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
        int count;
        size_t countLen;
        sysctlbyname("hw.logicalcpu", &count, &countLen, nullptr, 0);
        return count;
#elif __linux__
        char line[128];

        FILE *file = fopen("/proc/cpuinfo", "r");
        int numCores = 0;
        while (fgets(line, 128, file) != nullptr) {
            if (strncmp(line, "processor", 9) == 0)
                numCores++;
        }
        fclose(file);
        log_debug << "Got number of processors, numProcs: " << numCores;
        return numCores;
#elif _WIN32
        return static_cast<int>(boost::thread::hardware_concurrency());
#endif
    }

}// namespace AwsMock::Core