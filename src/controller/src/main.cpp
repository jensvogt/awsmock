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
//
// C++ standard includes
#include <cstdlib>
#include <iostream>

// Boost includes
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

// AwsMock includes
#include <awsmock/controller/Controller.h>

#ifdef _WIN32
#define DEFAULT_CONFIG_FILE std::string("C:\\Program Files (x86)\\awsmock\\etc\\awsmock.json")
#else
#define DEFAULT_CONFIG_FILE std::string("/usr/local/awsmock/etc/awsmock.json")
#endif

// Allowed actions
static std::list<std::string> allowedActions() {
    return {
            "logLevel", "logs", "list", "enable", "disable", "start", "stop", "restart", "status",
            "export", "import", "clean", "clean-objects", "config", "ping", "loglevel"};
}

/**
 * Show help
 */
void ShowHelp(const boost::program_options::options_description &desc) {
    constexpr int leftIndent = 64;
    std::cout << std::endl
              << "AwsMock awsmockctl v" << PROJECT_VERSION << std::endl
              << std::endl
              << "Usage: " << std::endl
              << "  awsmockctl [Options] Commands" << std::endl
              << std::endl
              << "Options:" << std::endl
              << std::left << std::setw(leftIndent) << "  --help" << ": show this usage" << std::endl
              << std::left << std::setw(leftIndent) << "  --version" << ": show current version" << std::endl
              << std::left << std::setw(leftIndent) << "  --host" << ": connect to manager on 'host'" << std::endl
              << std::left << std::setw(leftIndent) << "  --port" << ": use 'port' for the manager connection" << std::endl
              << std::endl
              << "Commands: " << std::endl
              << std::left << std::setw(leftIndent) << "  status: shows the status of all available applications and lambdas." << std::endl
              << std::left << std::setw(leftIndent) << "  list [applications|lambdas]" << ": list all available applications or lambdas. If no argument is given, both are listed." << std::endl
              << std::left << std::setw(leftIndent) << "  enable [applications|lambdas|<applications...>|<lambda...>]" << ": enable the given application(s) or lambda(s). If no argument is given, enables all applications and lambdas. Several applications should be separated by spaces." << std::endl
              << std::left << std::setw(leftIndent) << "  disable [applications|lambdas|<applications...>|<lambda...>]" << ": disable the given application(s) or lambda(s). If no argument is given, disables all applications and lambdas. Several applications should be separated by spaces." << std::endl
              << std::left << std::setw(leftIndent) << "  start [applications|lambdas|<applications...>|<lambda...>]" << ": starts the given application(s) or lambda(s). If no argument is given, starts all applications and lambdas. Several applications should be separated by spaces." << std::endl
              << std::left << std::setw(leftIndent) << "  stop [applications|lambdas|<applications...>|<lambda...>]" << ": stops the given application(s) or lambda(s). If no argument is given, stops all applications and lambdas. Several applications should be separated by spaces." << std::endl
              << std::left << std::setw(leftIndent) << "  restart [applications|lambdas|<applications...>|<lambda...>]" << ": restarts the given application(s) or lambda(s). If no argument is given, restarts all applications and lambdas. Several applications should be separated by spaces." << std::endl;
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
              << "\nValid log levels are: fatal, error, warning, info, debug, verbose." << std::endl
              << std::endl;
}

/**
 * @brief Main routine.
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