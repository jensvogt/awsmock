//
// Created by vogje01 on 10/23/23.
//

#include <awsmock/controller/Controller.h>
#include <awsmock/dto/module/ExportInfrastructureRequest.h>
#include <awsmock/dto/module/ListModuleNamesResponse.h>

namespace AwsMock::Controller {

    AwsMockCtl::AwsMockCtl() {

        // Initialize database
        _host = Core::Configuration::instance().GetValueString("awsmock.gateway.http.host");
        _port = Core::Configuration::instance().GetValueInt("awsmock.gateway.http.port");
        _baseUrl = "http://" + _host + ":" + std::to_string(_port);

        // Get user/clientId/region
        _user = Core::Configuration::instance().GetValueString("awsmock.user");
        _clientId = Core::Configuration::instance().GetValueString("awsmock.access.client-id");
        _region = Core::Configuration::instance().GetValueString("awsmock.region");
    }

    void AwsMockCtl::Initialize(const boost::program_options::variables_map &vm, const std::vector<std::string> &commands) {
        _commands = commands;
        _vm = vm;
    }

    void AwsMockCtl::Run() {

        if (std::ranges::find(_commands, "list") != _commands.end()) {

            ListServices();

        } else if (std::ranges::find(_commands, "config") != _commands.end()) {

            GetConfig();

        } else if (std::ranges::find(_commands, "loglevel") != _commands.end()) {

            std::string loglevel;
            for (const auto &command: _commands) {
                if (command != "loglevel") {
                    loglevel = command;
                }
            }
            SetLogLevel(loglevel);

        }
#ifdef HAS_SYSTEMD
        else if (std::ranges::find(_commands, "logs") != _commands.end()) {

            ShowServiceLogs();
        }
#endif
        else if (std::ranges::find(_commands, "start") != _commands.end()) {

            std::vector<Dto::Module::Module> modules;
            for (const auto &command: _commands) {
                if (command != "start" && command != "all") {
                    Dto::Module::Module module;
                    module.name = command;
                    module.status = Database::Entity::Module::ModuleState::RUNNING;
                    modules.emplace_back(module);
                }
            }

            if (modules.empty()) {
                modules = GetAllModules();
            }

            StartService(modules);

        } else if (std::ranges::find(_commands, "restart") != _commands.end()) {

            std::vector<Dto::Module::Module> modules;
            for (const auto &command: _commands) {
                if (command != "restart" && command != "all") {
                    Dto::Module::Module module;
                    module.name = command;
                    modules.emplace_back(module);
                }
            }

            if (modules.empty()) {
                modules = GetAllModules();
            }

            StartService(modules);

        } else if (std::ranges::find(_commands, "stop") != _commands.end()) {

            std::vector<Dto::Module::Module> modules;
            for (const auto &command: _commands) {
                if (command != "stop" && command != "all") {
                    Dto::Module::Module module;
                    module.name = command;
                    module.status = Database::Entity::Module::ModuleState::STOPPED;
                    modules.emplace_back(module);
                }
            }

            if (modules.empty()) {
                modules = GetAllModules();
            }

            StopService(modules);

        } else if (std::ranges::find(_commands, "import") != _commands.end()) {

            ImportInfrastructure();

        } else if (std::ranges::find(_commands, "export") != _commands.end()) {

            std::vector<std::string> modules;
            for (const auto &command: _commands) {
                if (command != "export" && command != "all") {
                    modules.emplace_back(command);
                }
            }

            if (modules.empty()) {
                modules = GetAllModuleNames();
            }

            bool pretty = _vm.count("pretty");
            bool includeObjects = _vm.count("include-objects");

            ExportInfrastructure(modules, pretty, includeObjects);

        } else if (std::ranges::find(_commands, "clean") != _commands.end()) {

            std::vector<Dto::Module::Module> modules;
            for (const auto &command: _commands) {
                if (command != "clean") {
                    Dto::Module::Module module;
                    module.name = command;
                    modules.emplace_back(module);
                }
            }

            if (modules.empty()) {
                modules = GetAllModules();
            }

            CleanInfrastructure(modules);

        } else if (std::ranges::find(_commands, "clean-objects") != _commands.end()) {

            std::vector<Dto::Module::Module> modules;
            for (const auto &command: _commands) {
                if (command != "clean-objects") {
                    Dto::Module::Module module;
                    module.name = command;
                    modules.emplace_back(module);
                }
            }
            CleanObjects(modules);

        } else if (std::ranges::find(_commands, "show-ftp-users") != _commands.end()) {

            if (_commands.size() != 2) {
                std::cerr << "ServerId missing!";
                return;
            }

            std::string serverId = _commands[1];
            ShowFtpUsers(serverId);

        } else if (std::ranges::find(_commands, "ping") != _commands.end()) {
            PingManager();
        }
    }

    void AwsMockCtl::ListServices() const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "list-modules");
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", {}, headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get module list: " << response.statusCode << " body: " << response.body << std::endl;
            return;
        }

        std::vector<Dto::Module::Module> modules = Dto::Module::Module::FromJsonList(response.body);
        std::cout << "Modules:" << std::endl;
        for (auto const &module: modules) {
            std::string sport = module.port > 0 ? std::to_string(module.port) : "";
            std::cout << "  " << std::setw(16) << std::left << module.name << std::endl;
        }
    }

    void AwsMockCtl::StartService(std::vector<Dto::Module::Module> &modules) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "start-modules");
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::put, _host, _port, "/", Dto::Module::Module::ToJson(modules), headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }

        modules = Dto::Module::Module::FromJsonList(response.body);
        for (const auto &module: modules) {
            if (module.port > 0) {
                std::cout << "Module " << module.name << "(" << module.port << ") started" << std::endl;
            } else {
                std::cout << "Module " << module.name << " started" << std::endl;
            }
        }
    }

    void AwsMockCtl::RestartService(std::vector<Dto::Module::Module> &modules) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "restart-modules");
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::put, _host, _port, "/", Dto::Module::Module::ToJson(modules), headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }

        modules = Dto::Module::Module::FromJsonList(response.body);
        for (const auto &module: modules) {
            if (module.port > 0) {
                std::cout << "Module " << module.name << "(" << module.port << ") restarted" << std::endl;
            } else {
                std::cout << "Module " << module.name << " restarted" << std::endl;
            }
        }
    }

    void AwsMockCtl::StopService(Dto::Module::Module::ModuleList &modules) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "stop-modules");

        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::put, _host, _port, "/", Dto::Module::Module::ToJson(modules), headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }

        modules = Dto::Module::Module::FromJsonList(response.body);
        for (const auto &module: modules) {
            if (module.port > 0) {
                std::cout << "Module " << module.name << "(" << module.port << ") stopped" << std::endl;
            } else {
                std::cout << "Module " << module.name << " stopped" << std::endl;
            }
        }
    }

#ifdef HAS_SYSTEMD
    void AwsMockCtl::ShowServiceLogs() {
        sd_journal *jd;
        int r = sd_journal_open(&jd, SD_JOURNAL_LOCAL_ONLY);
        if (r != 0) {
            std::cerr << "Failed to open journal:" << strerror(r) << std::endl;
            return;
        }

        r = sd_journal_add_match(jd, "_SYSTEMD_UNIT=awsmock.service", 0);
        if (r != 0) {
            std::cerr << "Failed to set matching entries: " << strerror(r) << std::endl;
            return;
        }

        r = sd_journal_seek_tail(jd);
        if (r != 0) {
            std::cerr << "Failed to got to end of journal:" << strerror(r) << std::endl;
            return;
        }

        for (;;) {
            const void *d;
            size_t l;
            r = sd_journal_next(jd);
            if (r < 0) {
                std::cerr << "Failed to iterate to next entry: " << strerror(-r) << std::endl;
                break;
            }
            if (r == 0) {
                // Reached the end, let's wait for changes, and try again
                r = sd_journal_wait(jd, static_cast<uint64_t>(-1));
                if (r < 0) {
                    std::cerr << "Failed to wait for changes: " << strerror(-r) << std::endl;
                    break;
                }
                continue;
            }
            r = sd_journal_get_data(jd, "MESSAGE", &d, &l);
            if (r < 0) {
                std::cerr << "Failed to read message field: " << strerror(-r) << std::endl;
                continue;
            }
            printf("%.*s\n", static_cast<int>(l), static_cast<const char *>(d) + 8);
        }
        sd_journal_close(jd);
    }
#endif

    void AwsMockCtl::SetLogLevel(const std::string &level) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "set-log-level");
        if (const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::post, _host, _port, "/", level, headers); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not set log level, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }
        std::cout << "Log level set to " << level << std::endl;
    }

    void AwsMockCtl::PingManager() const {
        const auto start = system_clock::now();

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "ping");
        if (const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", {}, headers); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "No response from manager, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }
        const long duration = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();
        log_debug << "Manager pinged (" << duration << "ms)";
    }

    void AwsMockCtl::GetConfig() const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "get-config");
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", {}, headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get configuration, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }

        const Dto::Module::GatewayConfig gatewayConfig = Dto::Module::GatewayConfig::FromJson(response.body);
        std::cout << "Config: " << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "Version: " << std::setw(32) << gatewayConfig.version << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "Endpoint: " << std::setw(32) << gatewayConfig.endpoint << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "Host: " << std::setw(32) << gatewayConfig.host << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "Port: " << std::setw(32) << gatewayConfig.port << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "Address: " << std::setw(32) << gatewayConfig.address << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "PID: " << std::setw(32) << gatewayConfig.pid << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "User: " << std::setw(32) << gatewayConfig.user << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "AccessId: " << std::setw(32) << gatewayConfig.accessId << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "ClientId: " << std::setw(32) << gatewayConfig.clientId << std::endl;
        std::cout << "  " << std::setw(16) << std::left << "DataDir: " << std::setw(32) << gatewayConfig.dataDir << std::endl;
        if (gatewayConfig.databaseActive) {
            std::cout << "  " << std::setw(16) << std::left << "Database: " << std::setw(32) << "true" << std::endl;
        } else {
            std::cout << "  " << std::setw(16) << std::left << "Database: " << std::setw(32) << "false" << std::endl;
        }
    }

    void AwsMockCtl::ExportInfrastructure(const std::vector<std::string> &modules, const bool pretty, const bool includeObjects) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "export");

        Dto::Module::ExportInfrastructureRequest moduleRequest;
        moduleRequest.modules = modules;
        moduleRequest.includeObjects = includeObjects;
        moduleRequest.prettyPrint = pretty;

        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::post, _host, _port, "/", moduleRequest.ToJson(), headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not export objects, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }

        std::cout << response.body;
    }

    void AwsMockCtl::ImportInfrastructure() const {

        std::string line;
        std::stringstream jsonString;
        while (std::getline(std::cin, line)) {
            jsonString << line;
        }

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "import");

        if (const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::post, _host, _port, "/", jsonString.str(), headers); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not import objects, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
        }
    }

    void AwsMockCtl::CleanInfrastructure(const Dto::Module::Module::ModuleList &modules) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "clean");

        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", Dto::Module::Module::ToJson(modules), headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not clean infrastructure, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }
        std::cout << response.body;
    }

    void AwsMockCtl::CleanObjects(Dto::Module::Module::ModuleList &modules) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "clean-objects");

        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::put, _host, _port, "/", Dto::Module::Module::ToJson(modules), headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }

        modules = Dto::Module::Module::FromJsonList(response.body);
        for (const auto &module: modules) {
            std::cout << "Objects of module " << module.name << " cleaned" << std::endl;
        }
    }

    void AwsMockCtl::ShowFtpUsers(const std::string &serverId) const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "show-ftp-users");

        const Dto::Transfer::Server server = {.serverId = serverId};

        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", server.ToJson(), headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }

        for (const std::vector<Dto::Transfer::User> users = Dto::Transfer::User::FromJsonList(response.body); const auto &user: users) {
            std::cout << "FTP user: " << user.userName << " password: " << user.password << std::endl;
        }
    }

    void AwsMockCtl::AddStandardHeaders(std::map<std::string, std::string> &headers, const std::string &action) const {
        headers["User"] = _user;
        headers["Region"] = _region;
        headers["x-awsmock-target"] = "module";
        headers["x-awsmock-action"] = action;
    }

    Dto::Module::Module::ModuleList AwsMockCtl::GetAllModules() const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "list-modules");
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", {}, headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get modules list, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return {};
        }

        return Dto::Module::Module::FromJsonList(response.body);
    }

    std::vector<std::string> AwsMockCtl::GetAllModuleNames() const {

        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "list-module-names");
        const Core::HttpSocketResponse response = Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", {}, headers);
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get modules list, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return {};
        }
        Dto::Module::ListModuleNamesResponse moduleResponse;
        moduleResponse.FromJson(response.body);
        return moduleResponse.moduleNames;
    }
}// namespace AwsMock::Controller