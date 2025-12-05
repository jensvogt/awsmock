//
// Created by vogje01 on 12/11/2022.
//

#ifndef AWSMOCK_CORE_SYSTEM_UTILS_H
#define AWSMOCK_CORE_SYSTEM_UTILS_H

// C++ includes
#include <filesystem>
#include <future>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

// Boost includes
#include "boost/asio/deadline_timer.hpp"
#include "boost/asio/ip/host_name.hpp"
#include "boost/asio/read.hpp"
#include "boost/asio/readable_pipe.hpp"
#include "boost/process.hpp"
#include "boost/thread.hpp"

// AwsMock includes
#include <awsmock/core/FileUtils.h>

#define RANDOM_PORT_MIN 32768
#define RANDOM_PORT_MAX 65536
#ifdef _WIN32
#define BUFSIZE 4096
#endif

namespace AwsMock::Core {

    /**
     * @brief System utils for command line execution and other system routines.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SystemUtils {
      public:

        /**
         * @brief Returns the current working directory.
         *
         * @return absolute path of the current work directory.
         */
        static std::string GetCurrentWorkingDir();

        /**
         * @brief Returns the home directory of the user
         *
         * @return absolute path of the home directory.
         */
        static std::string GetHomeDir();

        /**
         * @brief Returns the root directory for the current machine
         *
         * @par
         * On Linux and macOS this will return "/", on Windows "C:\".
         *
         * @return absolute path of the root directory.
         */
        static std::string GetRootDir();

        /**
         * @brief Returns the DNS host name of the server
         *
         * @return host name of the server
         */
        static std::string GetHostName();

        /**
         * @brief Returns a random port number between 32768 and 65536
         *
         * @return random port
         */
        static int GetRandomPort();

        /**
         * @brief Returns the PID of the current process
         *
         * @return PID of the current process
         */
        static int GetPid();

        /**
         * @brief Returns the number of CPU cores
         *
         * @return number of CPU cores
         */
        static int GetNumberOfCores();

        /**
         * @brief Returns the value of an environment variable or empty string, if not existent.
         *
         * @param name of the environment variable
         * @return value of the environment variable as string
         */
        static std::string GetEnvironmentVariableValue(const std::string &name);

        /**
         * @brief Returns true if environment variable exists.
         *
         * @param name of the environment variable
         * @return true if existent, otherwise false
         */
        static bool HasEnvironmentVariable(const std::string &name);

        /**
         * @brief Run command in a shell
         *
         * @param shellcmd command
         * @param args vector of string arguments
         * @param output output stream
         * @param error error stream
         */
        static void RunShellCommand(const std::string &shellcmd, const std::vector<std::string> &args, std::string &output, std::string &error);

        /**
         * @brief Run command in a shell
         *
         * @param shellcmd command
         * @param args vector of string arguments
         * @param input input stream
         * @param output output stream
         * @param error error stream
         */
        // static void RunShellCommand(const std::string &shellcmd, const std::vector<std::string> &args, const std::string &input, std::string &output, std::string &error);

        /**
         * @brief Return the next free port number
         *
         * @return next free port number
         */
        static int GetNextFreePort();
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_SYSTEM_UTILS_H
