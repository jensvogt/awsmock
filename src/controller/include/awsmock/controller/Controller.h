//
// Created by vogje01 on 10/23/23.
//

#ifndef AWSMOCK_CONTROLLER_CONTROLLER_H
#define AWSMOCK_CONTROLLER_CONTROLLER_H

// C includes
#ifdef HAS_SYSTEMD
#include <systemd/sd-daemon.h>
#include <systemd/sd-journal.h>
#endif

// C++ includes
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Boost includes
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/dto/module/model/GatewayConfig.h>
#include <awsmock/dto/module/model/Module.h>
#include <awsmock/dto/transfer/model/Server.h>
#include <awsmock/dto/transfer/model/User.h>
#include <awsmock/repository/ModuleDatabase.h>

namespace AwsMock::Controller {

    /**
     * @brief AwsMock controller
     *
     * @par
     * AwsMock controller, which sends commands to the awsmock manager. Default port is 4567, but can be changed in the awsmock properties file.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AwsMockCtl {

      public:

        /**
         * @brief Constructor
         */
        explicit AwsMockCtl();

        /**
         * @brief Initialization
         *
         * @param vm vector of command line options
         * @param commands vector of commands
         */
        void Initialize(const boost::program_options::variables_map &vm, const std::vector<std::string> &commands);

        /**
         * @brief Main method
         */
        void Run();

        /**
         * @brief List all available services
         */
        void ListServices() const;

        /**
         * @brief Start a module
         *
         * @param modules list of modules names
         */
        void StartService(std::vector<Dto::Module::Module> &modules) const;

        /**
         * @brief Restart a module
         *
         * @param modules list of modules names
         */
        void RestartService(std::vector<Dto::Module::Module> &modules) const;

        /**
         * @brief Stops a module
         *
         * @param modules list of modules names
         */
        void StopService(std::vector<Dto::Module::Module> &modules) const;

#ifdef HAS_SYSTEMD
        /**
         * @brief Show the logs
         */
        static void ShowServiceLogs();
#endif

        /**
         * @brief Sets the managers log level
         *
         * @param level log level
         */
        void SetLogLevel(const std::string &level) const;

        /**
         * @brief Returns the current AwsMock configuration
         */
        void GetConfig() const;

        /**
         * @brief Dumps the current infrastructure as JSON file to stdout.
         *
         * @param modules list of modules
         * @param pretty JSON pretty print (indent=4)
         * @param includeObjects include also objects
         */
        void ExportInfrastructure(const std::vector<std::string> &modules, bool pretty = true, bool includeObjects = false) const;

        /**
         * @brief Imports the current infrastructure from stdin
         */
        void ImportInfrastructure() const;

        /**
         * @brief Cleans the current infrastructure.
         *
         * @param modules list of modules
         */
        void CleanInfrastructure(const Dto::Module::Module::ModuleList &modules) const;

        /**
         * @brief Cleans the objects of the given modules
         *
         * @param modules list of modules
         */
        void CleanObjects(Dto::Module::Module::ModuleList &modules) const;

        /**
         * @brief Show FTP users.
         *
         * As the AWS CLI does not allow to see the users password, this is a workaround. It will send a FTP user list
         * request to the server and shows the currently defined FTP users.
         *
         * @param serverId ID of the FTP server.
         */
        void ShowFtpUsers(const std::string &serverId) const;

        /**
         * @brief Send a ping to the server and waits for the response
         */
        void PingManager() const;

      private:

        /**
         * @brief Add authorization header.
         *
         * @param headers headers
         * @param action action to perform
         */
        void AddStandardHeaders(std::map<std::string, std::string> &headers, const std::string &action) const;

        /**
         * @brief Get a list of all modules.
         *
         * @return list of all modules.
         */
        Dto::Module::Module::ModuleList GetAllModules() const;

        /**
         * @brief Get a list of all module names.
         *
         * @return list of all module names.
         */
        std::vector<std::string> GetAllModuleNames() const;

        /**
         * Commands
         */
        std::vector<std::string> _commands;

        /**
         * Command line options
         */
        boost::program_options::variables_map _vm;

        /**
         * Host
         */
        std::string _host;

        /**
         * Port
         */
        int _port;

        /**
         * Base URL
         */
        std::string _baseUrl;

        /**
         * User
         */
        std::string _user;

        /**
         * Client ID
         */
        std::string _clientId;

        /**
         * AWS region
         */
        std::string _region;
    };

}// namespace AwsMock::Controller

#endif// AWSMOCK_CONTROLLER_CONTROLLER_H
