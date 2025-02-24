// ===================================================================================
//
// Created by vogje01 on 21/12/2022.
// Copyright 2022 -2024 Dr. Jens Vogt
//
// This file is part of aws-mock.
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
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// AwsMock includes
#include <awsmock/core/config/Configuration.h>
#include <awsmock/server/Manager.h>
#include <awsmock/service/frontend/FrontendServer.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define DEFAULT_CONFIG_FILE "/usr/local/awsmock/etc/awsmock.yml"

/**
 * Main routine.
 *
 * @param argc number of command line arguments.
 * @param argv command line arguments.
 * @return system exit code.
 */
int main(int argc, char *argv[]) {

    // Initialize logging
    AwsMock::Core::LogStream::Initialize();

    // Declare the supported options.
    boost::program_options::options_description desc("Options");
    desc.add_options()("help", "produce help message")("version", "application version")("config", boost::program_options::value<std::string>(), "set configuration file")("loglevel", boost::program_options::value<std::string>(), "set log level")("logfile", boost::program_options::value<std::string>(), "set log file");

    boost::program_options::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    // Show usage
    if (vm.contains("help")) {
        std::cout << std::endl
                  << "AwsMock manager v" << AwsMock::Core::Configuration::GetVersion() << std::endl
                  << std::endl
                  << "Usage: " << std::endl
                  << "  awsmockmgr [Options]" << std::endl
                  << std::endl
                  << desc << std::endl;
        return 0;
    }

    // Show version
    if (vm.contains("version")) {
        std::cout << std::endl
                  << "AwsMock manager v" << AwsMock::Core::Configuration::GetVersion() << std::endl
                  << std::endl;
        return 0;
    }

    // Read configuration
    AwsMock::Core::Configuration &configuration = AwsMock::Core::Configuration::instance();
    if (vm.contains("config")) {
        configuration.SetFilename(vm["config"].as<std::string>());
    } else {
        configuration.SetFilename(DEFAULT_CONFIG_FILE);
    }

    // Set log level
    if (vm.contains("loglevel")) {
        auto value = vm["loglevel"].as<std::string>();
        AwsMock::Core::Configuration::instance().SetValueString("awsmock.logging.level", value);
        AwsMock::Core::LogStream::SetSeverity(value);
    } else {
        std::string level = AwsMock::Core::Configuration::instance().GetValueString("awsmock.logging.level");
        AwsMock::Core::LogStream::SetSeverity(level);
    }

    // Set log file
    if (vm.contains("logfile")) {
        auto value = vm["logfile"].as<std::string>();
        AwsMock::Core::Configuration::instance().SetValueString("awsmock.logging.file-name", value);
        AwsMock::Core::LogStream::SetFilename(value);
    }

    // Start HTTP frontend server
    AwsMock::Service::Frontend::FrontendServer server;
    boost::thread t{boost::ref(server)};
    t.detach();

    // Start manager
    AwsMock::Manager::Manager awsMockManager;
    awsMockManager.Initialize();
    awsMockManager.Run();

    return EXIT_SUCCESS;
}