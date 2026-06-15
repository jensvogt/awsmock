// ===================================================================================
//
// Created by vogje01 on 21/12/2022.
// Copyright 2022 -2026 Dr. Jens Vogt
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
#define DEFAULT_CONFIG_FILE std::string("C:\\Program Files\\awsmock\\etc\\awsmock.json")
#elif __APPLE__
#define DEFAULT_CONFIG_FILE std::string("/usr/local/awsmock/etc/awsmock.json")
#else
#define DEFAULT_CONFIG_FILE std::string("/usr/local/awsmock/etc/awsmock.json")
#endif

/**
 * Show help
 */
void ShowHelp(const boost::program_options::options_description &desc) {
    constexpr int leftIndent = 64;
    std::cout << std::endl
              << "AwsMock awsmockctl v" << APP_VERSION << std::endl
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
              << std::left << std::setw(leftIndent) << "  status" << ": shows the status of all available applications and lambdas." << std::endl
              << std::left << std::setw(leftIndent) << "  list" << ": list all available applications and lambdas." << std::endl
              << std::left << std::setw(leftIndent) << "  list-applications" << ": list all available applications." << std::endl
              << std::left << std::setw(leftIndent) << "  list-lambdas" << ": list all available lambda functions." << std::endl
              << std::left << std::setw(leftIndent) << "  enable-applications [<applications...>]" << ": enable the given application(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  enable-lambdas [<lambdas...>]" << ": enable the given lambda(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  disable-applications [<applications...>]" << ": disable the given application(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  disable-lambdas [<lambdas...>]" << ": disable the given lambda(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  start-applications [<applications...>]" << ": starts the given application(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  start-lambdas [<lambdas...>]" << ": starts the given lambda(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  stop-applications [<applications...>]" << ": stops the given application(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  stop-lambdas [<lambdas...>]" << ": stops the given lambda(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  restart-applications [<applications...>]" << ": restarts the given application(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  restart-lambdas [<lambdas...>]" << ": restarts the given lambda(s)." << std::endl
              << std::left << std::setw(leftIndent) << "  deploy-application <application name> <application package>" << ": deploys the given application. Depending on the runtime the application should be supplied as JAR or ZIP file" << std::endl
              << std::left << std::setw(leftIndent) << "  deploy-lambda <lambda name> <lambda package>" << ": deploys the given lambda function. Depending on the runtime the lambda function should be supplied as JAR or ZIP file" << std::endl;
#ifdef HAS_SYSTEMD
    std::cout << std::left << std::setw(leftIndent) << "  logs" << ": shows the manager logs" << std::endl;
#endif
    std::cout << std::left << std::setw(leftIndent) << "  set-loglevel <level> [<channel>]" << ": sets the manager log to level, if no channel is given, set all" << std::endl
              << std::left << std::setw(leftIndent) << "  get-loglevel [<channel>]" << ": shows the current log level for the given channel, if no channel is given, get all" << std::endl
              << std::left << std::setw(leftIndent) << "  config" << ": shows the gateway configuration" << std::endl
              << std::left << std::setw(leftIndent) << "  export [<modules>] [export-options]" << ": dumps the current infrastructure to stdout. Modules is a space separated list of module names." << std::endl
              << std::left << std::setw(leftIndent) << "  import" << ": imports the infrastructure from stdin." << std::endl
              << std::left << std::setw(leftIndent) << "  clean [modules]" << ": cleans the current infrastructure. Modules is a space separated list of module names." << std::endl
              << std::left << std::setw(leftIndent) << "  clean-objects [modules]" << ": cleans the objects from the module. Modules is a space separated list of module names." << std::endl
              << std::left << std::setw(leftIndent) << "  ping" << ": ping the manager." << std::endl
              << "\nExport options:\n"
              << std::left << std::setw(leftIndent) << "  --include-objects" << ": export objects as well" << std::endl
              << std::left << std::setw(leftIndent) << "  --pretty" << ": indent output" << std::endl
              << "\nLog levels:" << std::endl
              << "  fatal, error, warning, info, debug, trace." << std::endl
              << "\nNotes:" << std::endl
              << "  All command support list of applications or lambdas. If no argument is given, the corresponding command will use all applications/lambdas. Several applications/lambdas should be separated by spaces."
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
    Awsmock::Core::LogStream::Initialize();
    Awsmock::Core::LogStream::RemoveConsoleLogs();

    // Declare the supported options.
    boost::program_options::options_description desc("Options");
    desc.add_options()("pretty", "produce JSON pretty output");
    desc.add_options()("include-objects", "include objects in export");
    desc.add_options()("help", "produce help message");
    desc.add_options()("version", "application version");
    desc.add_options()("config", boost::program_options::value<std::string>(), "set configuration file");
    desc.add_options()("file", boost::program_options::value<std::string>(), "set deployment file");
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

    // Check length
    if (vm.empty() && commands.empty()) {
        ShowHelp(desc);
        return EXIT_SUCCESS;
    }

    // Show usage.
    if (vm.contains("help")) {
        ShowHelp(desc);
        return EXIT_SUCCESS;
    }

    // Show the version
    if (vm.contains("version")) {
        std::cout << std::endl
                  << "AwsMock awsmockctl v" << Awsmock::Core::Configuration::getVersion() << std::endl
                  << std::endl;
        return EXIT_SUCCESS;
    }

    // Read the configuration file.
    auto configFilePath = DEFAULT_CONFIG_FILE;
    if (vm.contains("config")) {
        configFilePath = vm["config"].as<std::string>();
    }
    Awsmock::Core::Configuration::instance().load(configFilePath);

    // Set the log level.
    if (vm.contains("loglevel")) {
        const std::string value = vm["loglevel"].as<std::string>();
        Awsmock::Core::Configuration::instance().set<std::string>("awsmock.logging.level", value);
        Awsmock::Core::LogStream::SetSeverity(value);
    } else {
        Awsmock::Core::LogStream::SetSeverity("warning");
    }

    // Set the log file
    if (Awsmock::Core::Configuration::instance().get<bool>("awsmock.logging.file-active")) {
        auto logDir = Awsmock::Core::Configuration::instance().get<std::string>("awsmock.logging.dir");
        auto prefix = Awsmock::Core::Configuration::instance().get<std::string>("awsmock.logging.prefix");
        int size = Awsmock::Core::Configuration::instance().get<int>("awsmock.logging.file-size");
        int count = Awsmock::Core::Configuration::instance().get<int>("awsmock.logging.file-count");
        Awsmock::Core::LogStream::AddFile(logDir, prefix, size, count);
    }

    if (vm.contains("host")) {
        Awsmock::Core::Configuration::instance().set<std::string>("awsmock.gateway.http.host", vm["host"].as<std::string>());
    }

    if (vm.contains("port")) {
        Awsmock::Core::Configuration::instance().set<int>("awsmock.gateway.http.port", vm["port"].as<int>());
    }

    // Check command
    bool found = false;
    const std::string &action = commands.front();
    if (const std::vector<std::string> availableCommands = Awsmock::Controller::CommandTypeList(); !std::ranges::contains(availableCommands, action)) {
        std::cerr << std::endl
                  << "Unknown command: " << action << std::endl;
        ShowHelp(desc);
        return EXIT_FAILURE;
    }

    // Start manager
    Awsmock::Controller::AwsMockCtl awsMockCtl;
    awsMockCtl.Initialize(vm, commands);
    awsMockCtl.Run();

    return EXIT_SUCCESS;
}

#ifdef WIN32
int APIENTRY mainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif
