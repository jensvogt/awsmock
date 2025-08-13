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
#include <awsmock/controller/Controller.h>

#ifdef _WIN32
#define DEFAULT_CONFIG_FILE "C:/Program Files (x86)/awsmock/etc/awsmock.j́son"
#else
#define DEFAULT_CONFIG_FILE "/usr/local/awsmock/etc/awsmock.json"
#endif

// Allowed actions
static std::list<std::string> allowedActions() {
    return {
            "logLevel", "logs",
            "list-applications", "enable-application", "disable-application", "start-application", "stop-application", "restart-application",
            "list-lambdas", "enable-lambda", "disable-lambda", "start-lambda", "stop-lambda", "restart-lambda",
            "export", "import", "clean", "clean-objects", "config", "ping", "loglevel"};
}

/**
 * Show help
 */
void ShowHelp(const boost::program_options::options_description &desc) {
    constexpr int leftIndent = 40;
    std::cout << std::endl
              << "AwsMock awsmockctl v" << AwsMock::Core::Configuration::GetVersion() << std::endl
              << std::endl
              << "Usage: " << std::endl
              << "  awsmockctl [Options] Commands" << std::endl
              << std::endl
              << desc << std::endl
              << "Commands: " << std::endl
              << std::left << std::setw(leftIndent) << "  list-applications" << ": lists all available applications" << std::endl
              << std::left << std::setw(leftIndent) << "  enable-application [<application>]" << ": enable the given application. If no argument is given, enables all applications." << std::endl
              << std::left << std::setw(leftIndent) << "  disable-application [<application>]" << ": disable the given application. If no argument is given, disables all applications." << std::endl
              << std::left << std::setw(leftIndent) << "  start-application [<application>]" << ": starts the given application. If no argument is given, starts all applications." << std::endl
              << std::left << std::setw(leftIndent) << "  stop-application [<application>]" << ": stops the given application. If no argument is given, stops all applications" << std::endl
              << std::left << std::setw(leftIndent) << "  restart-application [<application>]" << ": restarts the given application. If no argument is given, restarts all applications" << std::endl
              << std::left << std::setw(leftIndent) << "  list-lambdas" << ": lists all available lambdas" << std::endl
              << std::left << std::setw(leftIndent) << "  enable-lambda [<lambda>]" << ": enable the given lambda function. If no argument is given, enables all lambda functions." << std::endl
              << std::left << std::setw(leftIndent) << "  disable-lambda [<lambda>]" << ": disable the given lambda function. If no argument is given, disables all lambda functions." << std::endl
              << std::left << std::setw(leftIndent) << "  start-lambda [<lambda>]" << ": starts the given lambda. If no argument is given, starts all lambdas." << std::endl
              << std::left << std::setw(leftIndent) << "  stop-lambda [<lambda>]" << ": stops the given lambda. If no argument is given, stops all lambdas" << std::endl
              << std::left << std::setw(leftIndent) << "  restart-lambda [<lambda>]" << ": restarts the given lambda. If no argument is given, restarts all lambdas" << std::endl;
#ifdef HAS_SYSTEMD
    std::cout << std::left << std::setw(leftIndent) << "  logs" << ": shows the manager logs" << std::endl;
#endif
    std::cout << std::left << std::setw(leftIndent) << "  loglevel <level>" << ": sets the manager log to level" << std::endl
              << std::left << std::setw(leftIndent) << "  config" << ": shows the gateway configuration" << std::endl
              << std::left << std::setw(leftIndent) << "  export [<modules>] [export-options]" << ": dumps the current infrastructure to stdout. Modules is a space separated list of module names." << std::endl
              << std::left << std::setw(leftIndent) << "  import" << ": imports the infrastructure from stdin." << std::endl
              << std::left << std::setw(leftIndent) << "  clean [modules]" << ": cleans the current infrastructure. Modules is a space separated list of module names." << std::endl
              << std::left << std::setw(leftIndent) << "  clean-objects [modules]" << ": cleans the objects from the module. Modules is a space separated list of module names." << std::endl
              << std::left << std::setw(leftIndent) << "  ping" << ": ping the manager." << std::endl
              << "\nExport options:\n"
              << std::left << std::setw(leftIndent) << "  --include-objects" << ": export objects as well" << std::endl
              << std::left << std::setw(leftIndent) << "  --pretty" << ": indent output" << std::endl
              << std::left << std::setw(leftIndent) << "  --host" << ": awsmock host name, default: localhost" << std::endl
              << std::left << std::setw(leftIndent) << "  --port" << ": awsmock port, default: 4566" << std::endl
              << "\nValid modules are: all, s3, sqs, sns, lambda, transfer, cognito, dynamodb, kms, secretsmanager, sts." << std::endl
              << "\nValid log levels are: fatal, error, warning, info, debug, verbose." << std::endl
              << std::endl;
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
    AwsMock::Core::LogStream::RemoveConsoleLogs();

    // Declare the supported options.
    boost::program_options::options_description desc("Options");
    desc.add_options()("pretty", "produce JSON pretty output");
    desc.add_options()("include-objects", "include objects in export");
    desc.add_options()("help", "produce help message");
    desc.add_options()("version", "application version");
    desc.add_options()("config", boost::program_options::value<std::string>(), "set configuration file");
    desc.add_options()("loglevel", boost::program_options::value<std::string>(), "set log level");
    desc.add_options()("logfile", boost::program_options::value<std::string>(), "set log file");
    desc.add_options()("host", boost::program_options::value<std::string>(), "awsmock host name");
    desc.add_options()("port", boost::program_options::value<int>(), "awsmock port");

    // Get command line options.
    boost::program_options::variables_map vm;
    const boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    store(parsed, vm);
    notify(vm);

    // Get commands.
    const std::vector<std::string> commands = collect_unrecognized(parsed.options, boost::program_options::include_positional);

    // Show usage.
    if (vm.contains("help")) {
        ShowHelp(desc);
        return EXIT_SUCCESS;
    }

    // Show the version
    if (vm.contains("version")) {
        std::cout << std::endl
                  << "AwsMock awslocal v" << AwsMock::Core::Configuration::GetVersion() << std::endl
                  << std::endl;
        return EXIT_SUCCESS;
    }

    // Read the configuration file.
    AwsMock::Core::Configuration &configuration = AwsMock::Core::Configuration::instance();
    if (vm.contains("config")) {
        configuration.SetFilename(vm["config"].as<std::string>());
    } else {
        configuration.SetFilename(DEFAULT_CONFIG_FILE);
    }

    // Set the log level.
    if (vm.contains("loglevel")) {
        const std::string value = vm["loglevel"].as<std::string>();
        AwsMock::Core::Configuration::instance().SetValue<std::string>("awsmock.logging.level", value);
        AwsMock::Core::LogStream::SetSeverity(value);
    } else {
        AwsMock::Core::LogStream::SetSeverity("warn");
    }

    // Set the log file
    if (AwsMock::Core::Configuration::instance().GetValue<bool>("awsmock.logging.file-active")) {
        auto logDir = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.logging.dir");
        auto prefix = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.logging.prefix");
        int size = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.logging.file-size");
        int count = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.logging.file-count");
        AwsMock::Core::LogStream::AddFile(logDir, prefix, size, count);
    }

    if (vm.contains("host")) {
        AwsMock::Core::Configuration::instance().SetValue<std::string>("awsmock.gateway.http.host", vm["host"].as<std::string>());
    }

    if (vm.contains("port")) {
        AwsMock::Core::Configuration::instance().SetValue<int>("awsmock.gateway.http.port", vm["port"].as<int>());
    }

    // Check command
    bool found = false;
    const std::string &action = commands.front();
    for (const std::string &s: allowedActions()) {
        if (s == action) {
            found = true;
            break;
        }
    }
    if (!found) {
        std::cerr << std::endl
                  << "Unknown command: " << action << std::endl;
        ShowHelp(desc);
        return EXIT_FAILURE;
    }

    // Start manager
    AwsMock::Controller::AwsMockCtl awsMockCtl;
    awsMockCtl.Initialize(vm, commands);
    awsMockCtl.Run();

    return EXIT_SUCCESS;
}

#ifdef WIN32
int APIENTRY mainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif