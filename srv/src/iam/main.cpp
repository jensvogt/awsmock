// C++ includes
#include <csignal>
#include <filesystem>
#include <iostream>

// Boost includes
#include <boost/program_options.hpp>

// Awsmock includes
#include "RepositoryFactory.h"

#include <Configuration.h>
#include <LogStream.h>
#include <SqsServer.h>
#include <Version.h>

#define DEFAULT_CONFIGURATION_FILE "/etc/awsmock/awsmock.json"
#define DEFAULT_LOG_LEVEL          "info"
#define DEFAULT_SOCKET_PATH        "/var/run/awsmock-sqs.sock"

namespace po = boost::program_options;

static Awsmock::sqs::SqsServer *g_server = nullptr;

static void onSignal(const int) {
    if (g_server) g_server->stop();
}

struct CliOptions {
    std::string socketPath;
    std::string configFile;
    std::string logLevel;
    bool consoleLog{true};
    bool fileLog{false};
};

std::optional<CliOptions> parseCommandLine(int argc, char *argv[]) {

    CliOptions opts;

    po::options_description general("General options", 120, 50);
    general.add_options()
            ("help,h", "Show this help message")
            ("version,v", "Show version information")
            ("config,c", po::value<std::string>(&opts.configFile)->default_value(DEFAULT_CONFIGURATION_FILE), "Path to JSON configuration file")
            ("socket,s", po::value<std::string>(&opts.socketPath)->default_value(DEFAULT_SOCKET_PATH), "Unix domain socket path");

    po::options_description logging("Logging options", 120, 50);
    logging.add_options()
            ("loglevel,l", po::value<std::string>(&opts.logLevel)->default_value(DEFAULT_LOG_LEVEL), "Log level (trace|debug|info|warning|error|fatal)")
            ("console-log", po::value<bool>(&opts.consoleLog)->default_value(true)->implicit_value(true), "Enable console logging")
            ("file-log", po::value<bool>(&opts.fileLog)->default_value(false)->implicit_value(true), "Enable file logging");

    po::options_description all("awsmock-sqs options");
    all.add(general).add(logging);

    try {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(all).run(), vm);

        if (vm.contains("help")) {
            std::cout << "awsmock-sqs v" << APP_VERSION << " - SQS service process\n\n" << all << "\n";
            return std::nullopt;
        }

        if (vm.contains("version")) {
            std::cout << "awsmock-sqs version " << APP_VERSION << "\n";
            return std::nullopt;
        }

        po::notify(vm);

    } catch (const po::error &e) {
        std::cerr << "Command line error: " << e.what() << "\n";
        std::cerr << "Use --help for usage information.\n";
        return std::nullopt;
    }

    return opts;
}

int initializeDatabase(const Awsmock::Core::Configuration &cfg) {

    try {

        // Choose backend from config
        if (const auto backend = cfg.getOr<std::string>("awsmock.database.backend", "mongodb"); backend == "memory") {
            log_info << "Using in-memory database";
            Awsmock::Database::RepositoryFactory::instance().initialize(Awsmock::Database::BackendType::MEMORY);
        } else {
            log_info << "Using MongoDB database";
            Awsmock::Database::Database::instance().initialize();
            Awsmock::Database::RepositoryFactory::instance().initialize(Awsmock::Database::BackendType::MONGODB);
        }

    } catch (std::exception &e) {
        log_error << "Failed to initialize database: " << e.what();
        return 1;
    }
    return 0;
}

int main(const int argc, char *argv[]) {

    const auto cliOpts = parseCommandLine(argc, argv);
    if (!cliOpts) return 0;

    auto &cfg = Awsmock::Core::Configuration::instance();

    if (std::filesystem::exists(cliOpts->configFile)) {
        try {
            cfg.load(std::filesystem::path(cliOpts->configFile));
        } catch (const std::exception &e) {
            std::cerr << "Failed to load config: " << e.what() << "\n";
            return 1;
        }
    }

    cfg.set<bool>("awsmock.logging.console-active", cliOpts->consoleLog);
    cfg.set<bool>("awsmock.logging.file-active", cliOpts->fileLog);

    Awsmock::Core::LogStream::Initialize();
    Awsmock::Core::LogStream::SetSeverity(cfg.getOr<std::string>("awsmock.logging.level", cliOpts->logLevel));

    // ── Initialize Database ─────────────────────────────
    if (const int error = initializeDatabase(cfg); error != 0) return error;

    std::signal(SIGTERM, onSignal);
    std::signal(SIGINT, onSignal);

    static Awsmock::sqs::SqsServer server(cliOpts->socketPath);
    g_server = &server;
    server.start();

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);
    int sig = 0;
    sigwait(&mask, &sig);

    server.stop();
    return 0;
}
