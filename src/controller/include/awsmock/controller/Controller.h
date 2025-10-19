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
#include <boost/json.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/dto/apps/internal/DisableAllApplicationRequest.h>
#include <awsmock/dto/apps/internal/DisableApplicationRequest.h>
#include <awsmock/dto/apps/internal/EnableAllApplicationRequest.h>
#include <awsmock/dto/apps/internal/EnableApplicationRequest.h>
#include <awsmock/dto/apps/internal/ListApplicationCountersRequest.h>
#include <awsmock/dto/apps/internal/RestartApplicationRequest.h>
#include <awsmock/dto/apps/internal/StartAllApplicationsRequest.h>
#include <awsmock/dto/apps/internal/StartApplicationRequest.h>
#include <awsmock/dto/apps/internal/StopAllApplicationsRequest.h>
#include <awsmock/dto/apps/internal/StopApplicationRequest.h>
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/lambda/internal/DisableAllLambdasRequest.h>
#include <awsmock/dto/lambda/internal/DisableLambdaRequest.h>
#include <awsmock/dto/lambda/internal/EnableAllLambdasRequest.h>
#include <awsmock/dto/lambda/internal/EnableLambdaRequest.h>
#include <awsmock/dto/lambda/internal/ListFunctionCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListLambdaInstanceCountersRequest.h>
#include <awsmock/dto/lambda/internal/ListLambdaInstanceCountersResponse.h>
#include <awsmock/dto/lambda/internal/StartAllLambdasRequest.h>
#include <awsmock/dto/lambda/internal/StartLambdaRequest.h>
#include <awsmock/dto/lambda/internal/StopAllLambdasRequest.h>
#include <awsmock/dto/lambda/internal/StopLambdaRequest.h>
#include <awsmock/dto/lambda/model/Function.h>
#include <awsmock/dto/module/ExportInfrastructureRequest.h>
#include <awsmock/dto/module/ListModuleNamesResponse.h>
#include <awsmock/dto/module/model/GatewayConfig.h>
#include <awsmock/dto/module/model/Module.h>
#include <awsmock/dto/transfer/model/Server.h>
#include <awsmock/dto/transfer/model/User.h>
#include <awsmock/repository/ModuleDatabase.h>

namespace AwsMock::Controller {

    /**
     * @brief Command types
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class CommandType {
        CONFIG,
        LOG_LEVEL,
        LIST,
        STATUS,
        ENABLE,
        DISABLE,
        START,
        RESTART,
        STOP,
        IMPORT,
        EXPORT,
        CLEAN,
        CLEAN_OBJECTS,
        LOGS,
        PING,
        UNKNOWN
    };

    static std::map<CommandType, std::string> CommandTypeNames{
            {CommandType::CONFIG, "config"},
            {CommandType::LOG_LEVEL, "log-level"},
            {CommandType::LIST, "list"},
            {CommandType::STATUS, "status"},
            {CommandType::ENABLE, "enable"},
            {CommandType::DISABLE, "disable"},
            {CommandType::START, "start"},
            {CommandType::RESTART, "restart"},
            {CommandType::STOP, "stop"},
            {CommandType::IMPORT, "import"},
            {CommandType::EXPORT, "export"},
            {CommandType::CLEAN, "clean"},
            {CommandType::CLEAN_OBJECTS, "clean-objects"},
            {CommandType::PING, "ping"},
            {CommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string CommandTypeToString(const CommandType &commandType) {
        return CommandTypeNames[commandType];
    }

    [[maybe_unused]] static CommandType CommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: CommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return CommandType::UNKNOWN;
    }

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
         * @brief Start one or more applications
         *
         * @param applications list of application names
         */
        void StartApplications(const std::vector<Dto::Apps::Application> &applications) const;

        /**
         * @brief Start all applications
         */
        void StartAllApplications() const;

        /**
         * @brief Enable one or more applications
         *
         * @param applications list of applications
         */
        void EnableApplications(const std::vector<Dto::Apps::Application> &applications) const;

        /**
         * @brief Disable all applications
         */
        void EnableAllApplications() const;

        /**
         * @brief Disable one or more applications
         *
         * @param applications list of applications
         */
        void DisableApplications(const std::vector<Dto::Apps::Application> &applications) const;

        /**
         * @brief Disable all applications
         */
        void DisableAllApplications() const;

        /**
         * @brief Restart one or more applications
         *
         * @param applications list of applications
         */
        void RestartApplications(const std::vector<Dto::Apps::Application> &applications) const;

        /**
         * @brief Restart all applications
         */
        void RestartAllApplications() const;

        /**
         * @brief Stops one or more applications
         *
         * @param applications list of applications
         */
        void StopApplications(const std::vector<Dto::Apps::Application> &applications) const;

        /**
         * @brief Stops all applications
         */
        void StopAllApplications() const;

        /**
         * @brief Enable one or more lambdas
         *
         * @param lambdas list of lambdas
         */
        void EnableLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const;

        /**
         * @brief Disable all lambdas
         */
        void EnableAllLambdas() const;

        /**
         * @brief Disable one or more lambdas
         *
         * @param lambdas list of lambdas
         */
        void DisableLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const;

        /**
         * @brief Disable all lambdas
         */
        void DisableAllLambdas() const;

        /**
         * @brief Start one or more lambdas
         *
         * @param lambdas list of application names
         */
        void StartLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const;

        /**
         * @brief Start all lambdas
         */
        void StartAllLambdas() const;


        /**
         * @brief Restart one or more lambdas
         *
         * @param lambdas list of application names
         */
        void RestartLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const;

        /**
         * @brief Restart all lambdas
         */
        void RestartAllLambdas() const;

        /**
         * @brief Stop one or more lambdas
         *
         * @param lambdas list of application names
         */
        void StopLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const;

        /**
         * @brief Stop all lambdas
         */
        void StopAllLambdas() const;

#ifdef HAS_SYSTEMD
        /**
         * @brief Show the logs
         */
        static void ShowServiceLogs();
#endif

        /**
         * @brief Sets the manager's log level
         *
         * @param level log level
         */
        void SetLogLevel(const std::string &level) const;

        /**
         * @brief Returns the current AwsMock configuration
         */
        void GetConfig() const;

        /**
         * @brief Dumps the current infrastructure as a JSON file to stdout.
         *
         * @param modules list of modules
         * @param pretty JSON pretty print (indent=4)
         * @param includeObjects include also objects
         */
        void ExportInfrastructure(const std::vector<Dto::Module::Module> &modules, bool pretty = true, bool includeObjects = false) const;

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
         * @brief Send a ping to the server and waits for the response
         */
        void PingManager() const;

      private:

        /**
         * @brief Add an authorization header.
         *
         * @param headers headers
         * @param target awsmock target
         * @param action action to perform
         */
        void AddStandardHeaders(std::map<std::string, std::string> &headers, const std::string &target, const std::string &action) const;

        /**
         * @brief Get a list of applications
         *
         * @param commands command line arguments
         * @return list of applications
         */
        std::vector<Dto::Apps::Application> GetApplicationFromCommands(const std::vector<std::string> &commands);

        /**
         * @brief Get a list of lambdas
         *
         * @param commands command line arguments
         * @return list of lambdas
         */
        std::vector<Dto::Lambda::Function> GetLambdasFromCommand(const std::vector<std::string> &commands);

        /**
         * @brief Get a list of modules
         *
         * @param commands command line arguments
         * @return list of modules
         */
        std::vector<Dto::Module::Module> GetModulesFromCommand(const std::vector<std::string> &commands);

        /**
         * @brief Get a list of all applications.
         *
         * @return list of all applications.
         */
        [[nodiscard]] std::vector<Dto::Apps::Application> GetAllApplications() const;

        /**
         * @brief Get a list of all lambdas.
         *
         * @return list of all lambdas.
         */
        [[nodiscard]] std::vector<Dto::Lambda::Function> GetAllLambdas() const;

        /**
         * @brief Get a list of all modules.
         *
         * @return list of all modules.
         */
        [[nodiscard]] std::vector<Dto::Module::Module> GetAllModules() const;

        /**
         * @brief Returns a lits of lambda instance counters
         *
         * @param function lambda function
         * @return list of instances
         */
        std::vector<Dto::Lambda::InstanceCounter> GetLambdaInstances(const Dto::Lambda::Function &function) const;

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

        /**
         * List of applications
         */
        std::vector<Dto::Apps::Application> _applications;

        /**
         * List of lamba functions
         */
        std::vector<Dto::Lambda::Function> _lambdas;
    };

}// namespace AwsMock::Controller

#endif// AWSMOCK_CONTROLLER_CONTROLLER_H
