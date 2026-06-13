//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/server/Manager.h>

#include "awsmock/service/apigateway/ApiGatewayServer.h"

#ifdef __linux__
#include <malloc.h>
#endif
#ifdef _WIN32
extern HANDLE g_ServiceStopEvent;
#endif

namespace Awsmock::Manager {

    void Manager::Initialize() const {

        // Initialize scheduler
        Core::Scheduler::initialize(_ioc);

        // Start database
        InitializeDatabase();

        // Initialize logging
        InitializeLogging();

#ifdef __linux__
        // Periodically return freed memory to the OS on Linux.
        // glibc keeps freed blocks in per-arena pools; malloc_trim coalesces
        // them and releases contiguous free pages back via sbrk/madvise.
        Core::Scheduler::instance().AddTask("malloc-trim", [] { malloc_trim(0); }, 60);
#endif

        // Initialize websocket logging
        InitializeWebsocketLogging();

        // Write some info
        WriteInfoMessages();
    }

    void Manager::InitializeLogging() const {

        // Initialize log channels from configuration
        const std::shared_ptr<Database::IModuleRepository> moduleDatabase = Database::RepositoryFactory::instance().moduleRepository();
        if (Core::Configuration::instance().has("awsmock.logging.channels")) {
            for (const auto &channel: Core::Configuration::instance().getArrayOfObjects("awsmock.logging.channels")) {
                if (!channel.contains("name") || !channel.contains("level")) continue;
                const std::string &channelName = channel.at("name");
                const std::string &level = channel.at("level");
                std::string moduleName = channelName;
                std::ranges::transform(moduleName, moduleName.begin(), ::tolower);
                Core::LogStream::SetChannelSeverity(channelName, level);
                if (moduleDatabase->moduleExists(moduleName)) {
                    moduleDatabase->setModuleLogChannelAndLevel(moduleName, channelName, level);
                    log_trace << "Module log channel initialized, name: " << moduleName << " channel: " << channelName << " level: " << level;
                }
            }
        }
    }

    void Manager::InitializeWebsocketLogging() const {

        if (Core::Configuration::instance().get<bool>("awsmock.logging.websocket-active")) {
            const auto port = Core::Configuration::instance().get<unsigned int>("awsmock.logging.websocket-port");
            Core::LogStream::AddLoggingWebSocket(_ioc, port);
        }
    }

    void Manager::InitializeDatabase() const {

        // Get database variables
        const std::string databaseName = Core::Configuration::instance().getOr<std::string>("awsmock.mongodb.active", "awsmock");
        if (Core::Configuration::instance().getOr<bool>("awsmock.mongodb.active", false)) {

            Database::RepositoryFactory::instance().initialize(Database::BackendType::MONGODB, databaseName);

            // Create database indexes in a background thread
            Core::Scheduler::instance().AddOneTimeTask("create-indexes", [] {
                Database::RepositoryFactory::instance().createIndexes();
            });
            log_info << "Mongo database initialized";

        } else {
            Database::RepositoryFactory::instance().initialize(Database::BackendType::MEMORY, databaseName);
            log_info << "In-memory database initialized";
        }
    }

    void Manager::WriteInfoMessages() const {
        std::string boostVersion = BOOST_LIB_VERSION;
        Core::StringUtils::Replace(boostVersion, "_", ".");
        log_info << "Starting " << Core::Configuration::getAppName() << " " << Core::Configuration::getVersion() << ", pid: " << Core::SystemUtils::GetPid()
                 << ", loglevel: " << Core::Configuration::instance().get<std::string>("awsmock.logging.level") << ", boost: " << boostVersion;
        log_info << "Configuration file: " << Core::Configuration::instance().filePath();
        log_info << "Dockerized: " << std::boolalpha << Core::Configuration::instance().get<bool>("awsmock.dockerized");
    }

    void Manager::AutoLoad() const {

        // Check active flag
        if (!Core::Configuration::instance().get<bool>("awsmock.autoload.active")) {
            return;
        }

        // Load by directory has preference over a load by file
        const auto autoLoadDir = Core::Configuration::instance().get<std::string>("awsmock.autoload.dir");
        log_info << "Using autoload directory: " << autoLoadDir;
        if (Core::DirUtils::DirectoryExists(autoLoadDir) && !Core::DirUtils::DirectoryEmpty(autoLoadDir)) {
            for (const auto &file: Core::DirUtils::ListFilesByExtension(autoLoadDir, "json", true)) {
                if (const std::string jsonString = Core::FileUtils::ReadFile(file); !jsonString.empty()) {

                    // Create infrastructure object
                    Dto::Module::Infrastructure infrastructure;
                    infrastructure.FromJson(jsonString);

                    // Create import request
                    Dto::Module::ImportInfrastructureRequest importRequest;
                    importRequest.cleanFirst = false;
                    importRequest.infrastructure = infrastructure;

                    // Import infrastructure
                    Service::ModuleService{}.ImportInfrastructure(importRequest);
                    log_info << "Loaded infrastructure, filename: " << file;
                }
            }
        } else if (const auto autoLoadFile = Core::Configuration::instance().get<std::string>("awsmock.autoload.file"); Core::FileUtils::FileExists(autoLoadFile)) {
            if (const std::string jsonString = Core::FileUtils::ReadFile(autoLoadFile); !jsonString.empty()) {
                Dto::Module::Infrastructure infrastructure;
                infrastructure.FromJson(jsonString);
                Dto::Module::ImportInfrastructureRequest importRequest;
                importRequest.cleanFirst = false;
                importRequest.infrastructure = infrastructure;
                Service::ModuleService{}.ImportInfrastructure(importRequest);
                log_info << "Loaded infrastructure, filename: " << autoLoadFile;
            }
        }
        log_info << "Autoload finished";
    }

    void Manager::StopModules(Service::ModuleMap &moduleMap) const {
        log_info << "Stopping modules";

        // Stop scheduler
        log_info << "Found modules, count: " << moduleMap.GetSize();
        int i = 0;
        const std::shared_ptr<Database::IModuleRepository> moduleDatabase = Database::RepositoryFactory::instance().moduleRepository();
        for (std::vector<Database::Entity::Module::Module> modules = moduleDatabase->listModules(); auto &module: modules) {
            log_info << "Stopping module " << i << " module: " << module.name;
            if (module.state == Database::Entity::Module::ModuleState::RUNNING) {
                moduleDatabase->setState(module.name, Database::Entity::Module::ModuleState::STOPPED);
                if (moduleMap.HasModule(module.name)) {
                    moduleMap.GetModule(module.name)->Shutdown();
                    log_info << "Module " << i << ": " << module.name << " stopped";
                }
            }
            i++;
        }
        log_info << "All modules stopped, count: " << moduleMap.GetSize();
    }

    void Manager::LoadModulesFromConfiguration() const {

        using Database::Entity::Module::ModuleStatus;

        const std::shared_ptr<Database::IModuleRepository> moduleDatabase = Database::RepositoryFactory::instance().moduleRepository();
        for (const std::map<std::string, Database::Entity::Module::Module> existingModules = Database::ModuleMongoRepository::getExisting(); const auto &key: existingModules | std::views::keys) {
            log_trace << "Loading module, key: " << key << " status: " << std::boolalpha << Core::Configuration::instance().get<bool>("awsmock.modules." + key + ".active");
            EnsureModuleExisting(key);
            Core::Configuration::instance().get<bool>("awsmock.modules." + key + ".active") ? moduleDatabase->setStatus(key, ModuleStatus::ACTIVE) : moduleDatabase->setStatus(key, ModuleStatus::INACTIVE);
        }

        // Gateway
        EnsureModuleExisting("gateway");
        moduleDatabase->setStatus("gateway", Core::Configuration::instance().get<bool>("awsmock.gateway.active") ? ModuleStatus::ACTIVE : ModuleStatus::INACTIVE);

        // Monitoring
        EnsureModuleExisting("monitoring");
        moduleDatabase->setStatus("monitoring", Core::Configuration::instance().get<bool>("awsmock.monitoring.active") ? ModuleStatus::ACTIVE : ModuleStatus::INACTIVE);
    }

    void Manager::EnsureModuleExisting(const std::string &key) const {

        using Database::Entity::Module::ModuleState;
        using Database::Entity::Module::ModuleStatus;

        const std::shared_ptr<Database::IModuleRepository> moduleDatabase = Database::RepositoryFactory::instance().moduleRepository();
        if (!Database::RepositoryFactory::instance().moduleRepository()->moduleExists(key)) {
            Database::Entity::Module::Module m = {.name = key, .state = ModuleState::STOPPED, .status = ModuleStatus::ACTIVE};
            m = moduleDatabase->createOrUpdateModule(m);
            log_debug << "Created module, name: " << m.name;
        }
    }

    void Manager::Run(const bool isService) {

        // Load available modules from configuration file
        LoadModulesFromConfiguration();
        log_info << "Module configuration loaded";

        Core::Scheduler &scheduler = Core::Scheduler::initialize(_ioc);
        log_info << "Scheduler initialized";

        // Initialize monitoring
        const auto monitoringServer = std::make_shared<Service::MonitoringServer>(scheduler);
        log_info << "Monitoring server started";

        // Autoload the init files before modules start
        Core::Scheduler::instance().AddOneTimeTask("auto-loader", [this] { AutoLoad(); });

        const std::shared_ptr<Database::IModuleRepository> moduleDatabase = Database::RepositoryFactory::instance().moduleRepository();
        for (const std::vector<Database::Entity::Module::Module> modules = moduleDatabase->listModules(); const auto &module: modules) {
            log_debug << "Initializing module, name: " << module.name;
            if (module.name == "gateway" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::GatewayServer>(_ioc));
            } else if (module.name == "s3" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::S3Server>());
            } else if (module.name == "sqs" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::SQSServer>(scheduler));
            } else if (module.name == "sns" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::SNSServer>(scheduler));
            } else if (module.name == "lambda" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::LambdaServer>());
            } else if (module.name == "transfer" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::TransferServer>(scheduler, _ioc));
            } else if (module.name == "cognito" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::CognitoServer>(scheduler));
            } else if (module.name == "dynamodb" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::DynamoDbServer>());
            } else if (module.name == "kms" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::KMSServer>(scheduler));
            } else if (module.name == "ssm" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::SSMServer>(scheduler));
            } else if (module.name == "secretsmanager" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::SecretsManagerServer>());
            } else if (module.name == "application" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::ApplicationServer>());
            } else if (module.name == "api-gateway" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                Service::ModuleMap::instance().AddModule(module.name, std::make_shared<Service::ApiGatewayServer>(_ioc));
            }
        }
        log_info << "Modules started, count: " << Service::ModuleMap::instance().GetSize();

        // Start listener threads
        auto const maxThreads = std::thread::hardware_concurrency();
        for (auto i = 0; i < maxThreads; i++) {
            _threadGroup.create_thread([this] { return _ioc.run(); });
        }
        log_info << "Gateway starting, threads: " << maxThreads;

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        boost::asio::signal_set signals(_ioc, SIGINT, SIGTERM);
        signals.async_wait([&](beast::error_code const &ec, const int signal) {
            // Stop the `io_context`. This will cause `run()` to return immediately,
            // eventually destroying the `io_context` and all the sockets in it.
            if (!ec) {
                log_info << "Backend stopping on signal: " << signal;
                Core::EventBus::instance().sigShutdown();
                //StopModules(Service::ModuleMap::instance());
                log_info << "Backend modules stopped";
                _ioc.stop();
                log_info << "Backend IO context stopped";
                log_info << "So long, and thanks for all the fish!";
                exit(0);
            }
        });
        log_info << "Manager signal handler installed";
        log_info << "Manager initialized and started";

        // Start IO context
#ifdef _WIN32
        if (isService) {

            // Work guard prevents ioc from stopping when idle
            auto workGuard = boost::asio::make_work_guard(_ioc);

            // Separate thread watches the stop event and stops ioc when signaled
            std::thread stopWatcher([&]() {
                log_info << "Stop watcher thread started.";
                WaitForSingleObject(g_ServiceStopEvent, INFINITE);
                log_info << "Stop event received, stopping io_context.";
                workGuard.reset();
                _ioc.stop();
            });

            // Run io_context normally — blocks until ioc.stop() is called
            try {
                _ioc.run();
            } catch (const std::exception &e) {
                log_error << "io_context exception: " << e.what();
            }

            // Wait for watcher thread to finish
            if (stopWatcher.joinable()) {
                stopWatcher.join();
            }
            log_info << "Backend stopped.";

        } else {

            // Console mode — run until Ctrl+C
            try {
                _ioc.run();
            } catch (const std::exception &e) {
                log_error << "io_context exception: " << e.what();
            }
        }
#else
        _ioc.run();
#endif
    }

    void Manager::Stop() const {
        log_info << "Manager::Stop() called.";
        _ioc.stop();
    }
} // namespace Awsmock::Manager
