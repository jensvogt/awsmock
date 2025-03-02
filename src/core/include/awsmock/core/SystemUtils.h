//
// Created by vogje01 on 12/11/2022.
//

#ifndef AWSMOCK_CORE_SYSTEM_UTILS_H
#define AWSMOCK_CORE_SYSTEM_UTILS_H

// C includes
#include <fcntl.h>

// C++ includes
#include <array>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

// Boost includes
#include <boost/asio/ip/host_name.hpp>
#include <boost/thread/thread.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/RandomUtils.h>
#include <awsmock/core/StreamFilter.h>
#include <awsmock/core/exception/CoreException.h>

#define RANDOM_PORT_MIN 32768
#define RANDOM_PORT_MAX 65536

namespace AwsMock::Core {

    struct ExecResult {
        int status = -1;
        std::string output;
    };
    typedef ExecResult ExecResult;

    /**
     * @brief System utils for command line execution and other system routines.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SystemUtils {

      public:

        /**
         * @brief Execute system command and capture the stdout output result.
         *
         * @param command command string
         * @return command output.
         */
        static ExecResult Exec(const std::string &command);

        /**
         * @brief Execute system command and capture the stdout output result.
         *
         * @param command command string
         * @return command output.
         */
        static ExecResult Exec2(const std::string &command);

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
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_SYSTEM_UTILS_H
