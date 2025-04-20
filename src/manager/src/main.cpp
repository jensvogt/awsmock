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

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST
#define BOOST_BIND_GLOBAL_PLACEHOLDERS 1

// Boost includes
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#ifdef WIN32
#include <boost/application.hpp>
#include <boost/application/initializers.hpp>
#include <boost/application/service_setup.hpp>
#endif

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/server/Manager.h>
#include <awsmock/service/frontend/FrontendServer.h>

#define DEFAULT_LOG_PREFIX std::string("awsmock")
#define DEFAULT_LOG_LEVEL std::string("info")
#ifdef WIN32
#define DEFAULT_CONFIG_FILE std::string("C:\\Program Files (x86)\\awsmock\\etc\\awsmock.yml")
#define DEFAULT_SERVICE_PATH std::string("C:\\Program Files (x86)\\awsmock\\bin\\awsmockmgr.exe")
#else
#define DEFAULT_CONFIG_FILE "/usr/local/awsmock/etc/awsmock.yml"
#define DEFAULT_LOG_FILE "/usr/local/awsmock/log/awsmock.log"
#endif

/**
 * @brief Unix foreground application
 */
class Daemon {

  public:

    /**
     * @brief Constructor
     */
    Daemon() = default;

    /**
     * Stop callback
     */
    void Stop() {
        frontendThread.interrupt();
        frontendThread.join();
        awsMockManager.Stop();
    }

    /**
     * @brief Main routine.
     *
     * @par
     * Will never return. except on SIGINT or SIGTERM signals.
     *
     * @return
     */
    int Run() {

        // Start HTTP frontend server
        AwsMock::Service::Frontend::FrontendServer server;
        frontendThread = boost::thread{boost::ref(server)};
        frontendThread.detach();

        // Start manager
        awsMockManager.Initialize();
        awsMockManager.Run();

        return EXIT_SUCCESS;
    }

  private:

    boost::thread frontendThread;
    AwsMock::Manager::Manager awsMockManager;
};

#ifdef _WIN32

class WindowsWorker {

  public:

    /**
     * @brief Constructor that will receive a application context
     *
     * @param context Windows application context
     */
    explicit WindowsWorker(boost::application::context &context) : context_(context) {}

    /**
     * @brief Receives the Windows service stop signal.
     *
     * @return true when stopping
     */
    static bool Stop() {
        log_info << "Got stop signal, shutting down";
        worker.Stop();
        return true;
    }

    /**
     * @brief Define the application operator
     */
    int operator()() const {
        return worker.Run();
    }

  private:

    /**
     * Static windows worker
     */
    static Daemon worker;

    /**
     * Application context to hold aspects
     */
    boost::application::context &context_;
};
Daemon WindowsWorker::worker;

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
    desc.add_options()("config", boost::program_options::value<std::string>()->default_value(DEFAULT_CONFIG_FILE), "set configuration file");
    desc.add_options()("loglevel", boost::program_options::value<std::string>()->default_value(DEFAULT_LOG_LEVEL), "set log level");
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

#ifdef WIN32
    if (vm.contains("install")) {
        boost::system::error_code ec;
        boost::application::example::install_windows_service(
                boost::application::setup_arg(vm["name"].as<std::string>()),
                boost::application::setup_arg(vm["display"].as<std::string>()),
                boost::application::setup_arg(vm["description"].as<std::string>()),
                boost::application::setup_arg(vm["path"].as<std::string>()))
                .install(ec);
        if (ec) {
            std::cerr << "Could not install Windows service, error: " << ec.message() << std::endl;
            return 1;
        }
        std::cout << "Windows service installed" << std::endl;
        return 0;
    }

    if (vm.contains("check")) {
        boost::system::error_code ec;
        bool exist = boost::application::example::check_windows_service(boost::application::setup_arg(vm["name"].as<std::string>())).exist(ec);

        if (ec) {
            std::cerr << ec.message() << std::endl;
            return 1;
        }
        if (exist)
            std::cout << "The service " << vm["name"].as<std::string>() << " is installed!" << std::endl;
        else
            std::cout << "The service " << vm["name"].as<std::string>() << " is NOT installed!" << std::endl;
        return 0;
    }

    if (vm.contains("uninstall")) {
        boost::system::error_code ec;
        boost::application::example::uninstall_windows_service(
                boost::application::setup_arg(vm["name"].as<std::string>()),
                boost::application::setup_arg(vm["path"].as<std::string>()))
                .uninstall(ec);

        if (ec) {
            std::cerr << "Could not uninstall Windows service, error: " << ec.message() << std::endl;
            return 1;
        }
        std::cout << "Windows service uninstalled" << std::endl;
        return 0;
    }
#endif

    // Read configuration
    if (vm.contains("config")) {
        const auto configFilename = vm["config"].as<std::string>();
        AwsMock::Core::Configuration::instance().SetFilename(configFilename);
    }

    // Set log level
    if (vm.contains("loglevel")) {
        auto value = vm["loglevel"].as<std::string>();
        AwsMock::Core::Configuration::instance().SetValueString("awsmock.logging.level", value);
        AwsMock::Core::LogStream::SetSeverity(value);
    } else {
        const std::string level = AwsMock::Core::Configuration::instance().GetValueString("awsmock.logging.level");
        AwsMock::Core::LogStream::SetSeverity(level);
    }

    // Set log file
    if (AwsMock::Core::Configuration::instance().HasValue("awsmock.logging.dir") &&
        AwsMock::Core::Configuration::instance().HasValue("awsmock.logging.prefix")) {
        AwsMock::Core::LogStream::AddFile();
    }

#ifdef WIN32

    // Create a context application aspect pool
    boost::application::context app_context;

    WindowsWorker worker(app_context);

    // add termination handler
    boost::application::handler<>::callback termination_callback = boost::bind(&WindowsWorker::Stop);

    app_context.insert<boost::application::termination_handler>(boost::make_shared<boost::application::termination_handler_default_behaviour>(termination_callback));

    int result = 0;
    boost::system::error_code ec;
    if (vm.contains("foreground")) {

        // Run as foreground process
        result = boost::application::launch<boost::application::common>(worker, app_context, ec);
        if (ec) {
            log_error << "Windows foreground process failed, error: " << ec.message() << ", code: " << ec.value();
        }
    } else {
        // Run as Windows service
        result = boost::application::launch<boost::application::server>(worker, app_context, ec);
        if (ec) {
            log_error << "Windows service start failed, error: " << ec.message() << ", code: " << ec.value();
        }
    }
    return result;

#else

    log_debug << "Starting manager";

    Daemon worker;
    return worker.Run();

#endif
}

#ifdef _WIN32
int APIENTRY mainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif