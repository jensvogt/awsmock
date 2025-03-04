//
// Created by vogje01 on 21/12/2022.
// Copyright 2022 - 2024 Jens Vogt
//
// This file is part of awsmock (AWS Cloud Simulation).
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

// C++ standard includes
#include <cstdlib>
#include <iostream>

// Boost includes
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// AwsMock includes
#include <awsmock/awslocal/AwsLocal.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <minwindef.h>

#ifdef _WIN32
#define DEFAULT_CONFIG_FILE "C:/Program Files (x86)/awsmock/etc/awsmock.yml"
#else
#define DEFAULT_CONFIG_FILE "/usr/local/awsmock/etc/awsmock.yml"
#endif

// Allowed actions
static std::list<std::string> allowedActions() {
    return {"profile", "host", "port"};
}

/**
 * Show help
 */
void ShowHelp(const boost::program_options::options_description &desc) {
    constexpr int leftIndent = 40;
    std::cout << std::endl
              << "AwsMock awslocal v" << AwsMock::Core::Configuration::GetVersion() << std::endl
              << std::endl
              << "Usage: " << std::endl
              << "  awslocal [Options] Commands" << std::endl
              << std::endl
              << desc << std::endl
              << "\nCommands:\nAny AWS command" << std::endl;
}

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
    desc.add_options()("host", boost::program_options::value<std::string>()->default_value(DEFAULT_HOST), "AWS host name");
    desc.add_options()("port", boost::program_options::value<int>()->default_value(DEFAULT_PORT), "AWS port");
    desc.add_options()("profile", boost::program_options::value<std::string>()->default_value(DEFAULT_PROFILE), "AWS profile");
    desc.add_options()("help", "produce help message");

    // Get command line options.
    boost::program_options::variables_map vm;
    const boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    store(parsed, vm);
    notify(vm);

    // Show usage.
    if (vm.contains("help")) {
        ShowHelp(desc);
        return EXIT_SUCCESS;
    }

    // Show version
    if (vm.contains("version")) {
        std::cout << std::endl
                  << "AwsMock awslocal v" << AwsMock::Core::Configuration::GetVersion() << std::endl
                  << std::endl;
        return EXIT_SUCCESS;
    }

    // Read configuration.
    AwsMock::Core::Configuration &configuration = AwsMock::Core::Configuration::instance();
    if (vm.contains("config")) {
        configuration.SetFilename(vm["config"].as<std::string>());
    } else {
        configuration.SetFilename(DEFAULT_CONFIG_FILE);
    }

    // Get commands.
    const std::vector<std::string> commands = collect_unrecognized(parsed.options, boost::program_options::include_positional);

    // Start manager
    AwsMock::AwsLocal::AwsLocal awsLocal;
    awsLocal.Initialize(vm);
    awsLocal.Run(commands);

    return EXIT_SUCCESS;
}

#ifdef WIN32
int APIENTRY mainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif