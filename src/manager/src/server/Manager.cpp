//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/server/Manager.h>

namespace AwsMock::Manager {

    void CreateIndexes() {
        Database::ModuleDatabase::instance().CreateIndexes();
        log_debug << "Database indexes created";
    }

    void Manager::Initialize() {

        InitializeDatabase();
        std::string boostVersion = BOOST_LIB_VERSION;
        Core::StringUtils::Replace(boostVersion, "_", ".");
        log_info << "Starting " << Core::Configuration::GetAppName() << " " << Core::Configuration::GetVersion() << " pid: " << getpid()
                 << " loglevel: " << Core::Configuration::instance().GetValueString("awsmock.logging.level") << " boost version: " << boostVersion;
        log_info << "Configuration file: " << Core::Configuration::instance().GetFilename();
        log_info << "Dockerized: " << std::boolalpha << Core::Configuration::instance().GetValueBool("awsmock.dockerized");
    }

    void Manager::InitializeDatabase() {

        // Get database variables
        if (const Core::Configuration &configuration = Core::Configuration::instance(); configuration.GetValueBool("awsmock.mongodb.active")) {
            const std::string name = configuration.GetValueString("awsmock.mongodb.name");
            const std::string host = configuration.GetValueString("awsmock.mongodb.host");
            const std::string user = configuration.GetValueString("awsmock.mongodb.user");
            const std::string password = configuration.GetValueString("awsmock.mongodb.password");
            const int _port = configuration.GetValueInt("awsmock.mongodb.port");
            const int poolSize = configuration.GetValueInt("awsmock.mongodb.pool-size");

            // MongoDB URL
            const std::string url = "mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(_port) + "/?maxPoolSize=" + std::to_string(poolSize);
            mongocxx::uri _uri(url.c_str());
            log_info << "Using MongoDB database url: " << url;

            auto instance = std::make_unique<mongocxx::instance>();
            Database::ConnectionPool &pool = Database::ConnectionPool::instance();

            // Options
            const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
            pool.configure(std::move(instance), std::make_unique<mongocxx::pool>(std::move(_uri)));
            log_info << "MongoDB database initialized, version: " << mongocxx::v_noabi::options::server_api::version_to_string(api.get_version());

            // Create database indexes in a background thread
            boost::thread t{CreateIndexes};
            t.detach();

        } else {
            log_info << "In-memory database initialized";
        }
    }

    void Manager::AutoLoad() {
        if (Core::Configuration::instance().GetValueBool("awsmock.autoload.active")) {
            if (const std::string autoLoadDir = Core::Configuration::instance().GetValueString("awsmock.autoload.dir"); Core::DirUtils::DirectoryExists(autoLoadDir) && !Core::DirUtils::DirectoryEmpty(autoLoadDir)) {
                for (const auto &file: Core::DirUtils::ListFilesByExtension(autoLoadDir, "json")) {
                    if (const std::string jsonString = Core::FileUtils::ReadFile(file); !jsonString.empty()) {
                        Service::ModuleService _moduleService;
                        _moduleService.ImportInfrastructure(jsonString);
                        log_info << "Loaded infrastructure from " << file;
                    }
                }
            } else if (const std::string autoLoadFile = Core::Configuration::instance().GetValueString("awsmock.autoload.file"); Core::FileUtils::FileExists(autoLoadFile)) {
                if (const std::string jsonString = Core::FileUtils::ReadFile(autoLoadFile); !jsonString.empty()) {
                    Service::ModuleService _moduleService;
                    _moduleService.ImportInfrastructure(jsonString);
                    log_info << "Loaded infrastructure from " << autoLoadFile;
                }
            }
        }
    }

    void Manager::StopModules() {
        log_info << "Stopping services";

        Service::ModuleMap moduleMap = Service::ModuleMap::instance();
        Database::ModuleDatabase &moduleDatabase = Database::ModuleDatabase::instance();
        for (Database::Entity::Module::ModuleList modules = moduleDatabase.ListModules(); const auto &module: modules) {
            if (module.state == Database::Entity::Module::ModuleState::RUNNING) {
                log_info << "Stopping module: " << module.name;
                moduleDatabase.SetState(module.name, Database::Entity::Module::ModuleState::STOPPED);
                if (moduleMap.HasModule(module.name)) {
                    moduleMap.GetModule(module.name)->Shutdown();
                    log_debug << "Module " << module.name << " stopped";
                }
            }
        }
    }

    void Manager::LoadModulesFromConfiguration() {

        using Database::Entity::Module::ModuleStatus;

        const Core::Configuration &configuration = Core::Configuration::instance();
        Database::ModuleDatabase &moduleDatabase = Database::ModuleDatabase::instance();

        for (const std::map<std::string, Database::Entity::Module::Module> existingModules = Database::ModuleDatabase::GetExisting(); const auto &key: existingModules | std::views::keys) {
            EnsureModuleExisting(key);
            configuration.GetValueBool("awsmock.modules." + key + ".active") ? moduleDatabase.SetStatus(key, ModuleStatus::ACTIVE) : moduleDatabase.SetStatus(key, ModuleStatus::INACTIVE);
        }

        // Gateway
        EnsureModuleExisting("gateway");
        moduleDatabase.SetStatus("gateway", configuration.GetValueBool("awsmock.gateway.active") ? ModuleStatus::ACTIVE : ModuleStatus::INACTIVE);

        // Monitoring
        EnsureModuleExisting("monitoring");
        moduleDatabase.SetStatus("monitoring", configuration.GetValueBool("awsmock.monitoring.active") ? ModuleStatus::ACTIVE : ModuleStatus::INACTIVE);
    }

    void Manager::EnsureModuleExisting(const std::string &key) {

        using Database::Entity::Module::ModuleState;
        using Database::Entity::Module::ModuleStatus;

        if (!Database::ModuleDatabase::instance().ModuleExists(key)) {
            Database::Entity::Module::Module m = {.name = key, .state = ModuleState::STOPPED, .status = ModuleStatus::ACTIVE};
            Database::ModuleDatabase::instance().CreateModule(m);
        }
    }

    void Manager::Run() {

        // Load available modules from configuration file
        LoadModulesFromConfiguration();

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        boost::asio::io_service ios;
        boost::asio::signal_set signals(ios, SIGINT, SIGTERM);
        signals.async_wait([&](boost::beast::error_code const &, int) {
            // Stop the `io_context`. This will cause `run()` to return immediately,
            // eventually destroying the `io_context` and all the sockets in it.
            log_info << "Manager stopped on signal";
            StopModules();
            ios.stop();
        });

        Core::PeriodicScheduler scheduler(ios);
        auto monitoringServer = std::make_shared<Service::MonitoringServer>(scheduler);

        Service::ModuleMap moduleMap = Service::ModuleMap::instance();
        const Database::ModuleDatabase &moduleDatabase = Database::ModuleDatabase::instance();
        for (Database::Entity::Module::ModuleList modules = moduleDatabase.ListModules(); const auto &module: modules) {
            if (module.name == "gateway" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::GatewayServer>(ios));
            } else if (module.name == "s3" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::S3Server>(scheduler));
            } else if (module.name == "sqs" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::SQSServer>(scheduler));
            } else if (module.name == "sns" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::SNSServer>(scheduler));
            } else if (module.name == "lambda" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::LambdaServer>(scheduler));
            } else if (module.name == "transfer" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::TransferServer>(scheduler));
            } else if (module.name == "cognito" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::CognitoServer>(scheduler));
            } else if (module.name == "dynamodb" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::DynamoDbServer>(scheduler));
            } else if (module.name == "kms" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::KMSServer>(scheduler));
            } else if (module.name == "ssm" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::SSMServer>(scheduler));
            } else if (module.name == "secretsmanager" && module.status == Database::Entity::Module::ModuleStatus::ACTIVE) {
                moduleMap.AddModule(module.name, std::make_shared<Service::SecretsManagerServer>(scheduler));
            }
        }

        // Auto load init file
        AutoLoad();

        // Start listener threads
        const int numProcs = Core::SystemUtils::GetNumberOfCores();
        for (auto i = 0; i < numProcs; i++) {
            _threadGroup.create_thread([ObjectPtr = &ios] { return ObjectPtr->run(); });
        }

        // Start IO context
        ios.run();
        log_info << "So long, and thanks for all the fish!";
    }
}// namespace AwsMock::Manager
