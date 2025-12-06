// ===================================================================================
//
// Created by vogje01 on 21/12/2022.
// Copyright 2022 -2025 Dr. Jens Vogt
//
// This file is part of awsmock.
//
// aws-mock is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// aws-mock is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with aws-mock.  If not, see <http://www.gnu.org/licenses/>.
//
// ===================================================================================

// C++ standard includes
#include <cstdlib>
#include <iostream>

// Boost includes
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// AwsMock includes
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/server/Manager.h>
#include <awsmock/service/frontend/FrontendServer.h>

#define DEFAULT_LOG_PREFIX std::string("awsmock")
#define DEFAULT_LOG_LEVEL std::string("info")
#ifdef WIN32
#include <awsmock/WindowsService.h>
#else
#define DEFAULT_CONFIG_FILE "/usr/local/awsmock/etc/awsmock.json"
#define DEFAULT_LOG_FILE "/usr/local/awsmock/log/awsmock.log"
#endif

/**
 * Main routine.
 *
 * @param argc number of command line arguments.
 * @param argv command line arguments.
 * @return system exit code.
 */
int main(const int argc, char *argv[]) {
    // Initialize logging
    AwsMock::Core::LogStream::Initialize();

    // Declare the supported options.
    boost::program_options::options_description desc("Options");
    desc.add_options()("help", "produce help message");
    desc.add_options()("version", "application version");
    desc.add_options()("loglevel", boost::program_options::value<std::string>(), "set log level");
    desc.add_options()("config", boost::program_options::value<std::string>()->default_value(DEFAULT_CONFIG_FILE), "set configuration file");
    desc.add_options()("logfile", boost::program_options::value<std::string>()->default_value(DEFAULT_LOG_PREFIX), "set log file");
#ifdef _WIN32
    desc.add_options()("foreground", "run as foreground process");
    desc.add_options()("install", "install windows service");
    desc.add_options()("check", "check windows service");
    desc.add_options()("uninstall", "uninstall windows service");
    desc.add_options()("name", boost::program_options::value<std::string>()->default_value("awsmock"), "windows service name");
    desc.add_options()("description", boost::program_options::value<std::string>()->default_value("AWS Cloud Simulation"), "windows service description");
    desc.add_options()("display", boost::program_options::value<std::string>()->default_value("AWS Mock"), "windows service display name");
    desc.add_options()("path", boost::program_options::value<std::string>()->default_value(DEFAULT_SERVICE_PATH), "windows service path");
#endif

    boost::program_options::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    // Show usage
    if (vm.find("help") != vm.end()) {
        std::cout << std::endl
                << "AwsMock manager v" << AwsMock::Core::Configuration::GetVersion() << std::endl
                << std::endl
                << "Usage: " << std::endl
                << "  awsmockmgr [Options]" << std::endl
                << std::endl
                << desc << std::endl;
        return 0;
    }

    // Show the version
    if (vm.find("version") != vm.end()) {
        std::cout << std::endl
                << "AwsMock manager v" << AwsMock::Core::Configuration::GetVersion() << std::endl
                << std::endl;
        return 0;
    }

#ifdef WIN32
    // Install Windows service
    if (vm.find("install") != vm.end()) {
        if (vm["name"].as<std::string>().empty()) {
            std::cerr << "Service name is required" << std::endl;
            return 1;
        }
        InstallService(argv[0], vm["name"].as<std::string>(), vm["display"].as<std::string>());
        return 0;
    }

    // Uninstall Windows service
    if (vm.find("uninstall") != vm.end()) {
        if (vm["name"].as<std::string>().empty()) {
            std::cerr << "Service name is required" << std::endl;
            return 1;
        }
        UninstallService(vm["name"].as<std::string>());
        return 0;
    }
#endif

    // Read configuration, log to stderr, as we do not have logging yet
    if (vm.find("config") != vm.end()) {
        const std::string configFilename = vm["config"].as<std::string>();
        AwsMock::Core::Configuration::instance().SetFilename(configFilename);
    }

    // Set the log level
    if (vm.find("loglevel") != vm.end()) {
        auto value = vm["loglevel"].as<std::string>();
        AwsMock::Core::Configuration::instance().SetValue<std::string>("awsmock.logging.level", value);
        AwsMock::Core::LogStream::SetSeverity(value);
    } else {
        const auto level = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.logging.level");
        AwsMock::Core::LogStream::SetSeverity(level);
    }

    // Set the log file
    if (AwsMock::Core::Configuration::instance().GetValue<bool>("awsmock.logging.file-active")) {
        auto logDir = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.logging.dir");
        auto prefix = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.logging.prefix");
        int size = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.logging.file-size");
        int count = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.logging.file-count");
        AwsMock::Core::LogStream::AddFile(logDir, prefix, size, count);
    }

#ifdef WIN32

    // Run as a foreground process on windows
    if (vm.find("foreground") != vm.end()) {
        // Run the detached frontend server thread
        boost::thread frontendThread;
        AwsMock::Service::Frontend::FrontendServer server;
        frontendThread = boost::thread{boost::ref(server), false};
        frontendThread.detach();
        log_info << "Frontend server started.";

        // Start manager
        boost::asio::io_context ioc;
        AwsMock::Manager::Manager awsMockManager{ioc};
        awsMockManager.Initialize();
        log_info << "Backend server started.";
        awsMockManager.Run(false);

        return 0;
    }

    // Windows service needs a file logger
    auto logDir = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.logging.dir");
    auto prefix = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.logging.prefix");
    int size = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.logging.file-size");
    int count = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.logging.file-count");
    AwsMock::Core::LogStream::AddFile(logDir, prefix, size, count);

    // Windows service table entries
    constexpr SERVICE_TABLE_ENTRY serviceTable[] = {
        {const_cast<LPSTR>(DEFAULT_SERVICE_NAME), static_cast<LPSERVICE_MAIN_FUNCTIONA>(ServiceMain)},
        {nullptr, nullptr}
    };

    // Windows service start
    if (!StartServiceCtrlDispatcher(serviceTable)) {
        log_error << "StartServiceCtrlDispatcher failed, error: " << GetLastError();
        return 1;
    }

#else

    // Run the detached frontend server thread
    boost::thread frontendThread;
    AwsMock::Service::Frontend::FrontendServer server;
    frontendThread = boost::thread{boost::ref(server), false};
    frontendThread.detach();
    log_info << "Frontend server started.";

    // Start manager
    boost::asio::io_context ioc;
    AwsMock::Manager::Manager awsMockManager{ioc};
    awsMockManager.Initialize();
    log_info << "Backend server started.";
    awsMockManager.Run(false);

#endif

    return 0;
}
