//
// Created by vogje01 on 12/11/2022.
//

#ifndef AWSMOCK_CORE_SYSTEM_UTILS_H
#define AWSMOCK_CORE_SYSTEM_UTILS_H

// C++ includes
#include <filesystem>
#include <string>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

// Boost includes
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/read.hpp>
#include <boost/process.hpp>
#include <boost/thread/thread.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>

#define RANDOM_PORT_MIN 32768
#define RANDOM_PORT_MAX 65536

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
         * @brief Run command in a shell
         *
         * @param shellcmd command
         * @param args vector of string arguments
         * @param input input stream
         * @param output output stream
         * @param error error stream
         */
        static void RunShellCommand(const std::string &shellcmd, const std::vector<std::string> &args, const std::string &input, std::string &output, std::string &error);
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_SYSTEM_UTILS_H
