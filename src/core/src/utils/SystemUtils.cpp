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
        auto status = WEXITSTATUS(std::system(cmd.c_str()));
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
    }

    std::string SystemUtils::GetCurrentWorkingDir() {
        char result[PATH_MAX];
        const ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        const std::string path = {std::string(result, (count > 0) ? count : 0)};
        return path.substr(0, path.find_last_of('/'));
    }

    std::string SystemUtils::GetHomeDir() {
        std::string homeDir;
        if (getenv("HOME") != nullptr) {
            homeDir = std::string(getenv("HOME"));
        } else {
            homeDir = std::string(getpwuid(getuid())->pw_dir);
        }
        return homeDir;
    }

    std::string SystemUtils::GetHostName() {
        return boost::asio::ip::host_name();
    }

    int SystemUtils::GetRandomPort() {
        return RandomUtils::NextInt(RANDOM_PORT_MIN, RANDOM_PORT_MAX);
    }

    int SystemUtils::GetPid() {
        return getpid();
    }

    int SystemUtils::GetNumberOfCores() {
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
    }

}// namespace AwsMock::Core