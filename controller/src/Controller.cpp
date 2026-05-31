// Created by vogje01 on 10/23/23.
//

#include <awsmock/controller/Controller.h>

namespace AwsMock::Controller {

    AwsMockCtl::AwsMockCtl() {
        _host = Core::Configuration::instance().get<std::string>("awsmock.gateway.http.host");
        _port = Core::Configuration::instance().get<int>("awsmock.gateway.http.port");
        _baseUrl = "http://" + _host + ":" + std::to_string(_port);
        _user = Core::Configuration::instance().get<std::string>("awsmock.user");
        _clientId = Core::Configuration::instance().get<std::string>("awsmock.access.client-id");
        _accountId = Core::Configuration::instance().get<std::string>("awsmock.access.account-id");
        _region = Core::Configuration::instance().get<std::string>("awsmock.region");
    }

    void AwsMockCtl::Initialize(const boost::program_options::variables_map &vm, const std::vector<std::string> &commands) {
        _commands = commands;
        _vm = vm;
        _lambdas = GetAllLambdas();
        _applications = GetAllApplications();
    }

    Core::HttpSocketResponse AwsMockCtl::SendCommand(const std::string &target, const std::string &action, const std::string &body) const {
        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, target, action);
        return Core::HttpSocket::SendJson(boost::beast::http::verb::post, _host, _port, "/", body, headers);
    }

    Core::HttpSocketResponse AwsMockCtl::SendGetCommand(const std::string &target, const std::string &action, const std::string &body) const {
        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, target, action);
        return Core::HttpSocket::SendJson(boost::beast::http::verb::get, _host, _port, "/", body, headers);
    }

    void AwsMockCtl::PrintApplicationList() const {
        std::cout << "Applications: " << std::endl;
        for (const auto &application: _applications) {
            std::cout << "  " << std::setw(32) << std::left << application.name
                      << std::setw(10) << std::left << (application.enabled ? "ENABLED" : "DISABLED")
                      << std::setw(10) << std::left << Dto::Apps::AppsStatusTypeToString(application.status) << std::endl;
        }
    }

    void AwsMockCtl::PrintLambdaList() const {
        std::cout << "Lambdas: " << std::endl;
        for (const auto &lambda: _lambdas) {
            std::cout << "  " << std::setw(32) << std::left << lambda.functionName
                      << std::setw(10) << std::left << (lambda.enabled ? "ENABLED" : "DISABLED")
                      << std::setw(10) << std::left << Core::StringUtils::ToUpper(lambda.state) << std::endl;
        }
    }

    void AwsMockCtl::Run() {
        const std::string action = _commands[0];
        _commands.erase(_commands.begin());
        switch (CommandTypeFromString(action)) {

            case CommandType::CONFIG:
                return GetConfig();

            case CommandType::SET_LOG_LEVEL: {
                const std::string level = _commands[0];
                const std::string channel = _commands.size() > 1 ? _commands[1] : std::string{};
                SetLogLevel(level, channel);
                break;
            }

            case CommandType::GET_LOG_LEVEL:
                GetLogLevel();
                break;

            case CommandType::STATUS: {
                std::cout << "Applications: " << std::endl;
                std::cout << "  " << std::setw(32) << std::left << "Name"
                          << std::setw(10) << std::left << "Enabled"
                          << std::setw(10) << std::left << "Status"
                          << std::setw(12) << std::left << "Ports" << std::endl;
                for (const auto &application: _applications) {
                    std::cout << "  " << std::setw(32) << std::left << application.name
                              << std::setw(10) << std::left << (application.enabled ? "ENABLED" : "DISABLED")
                              << std::setw(10) << std::left << Dto::Apps::AppsStatusTypeToString(application.status);
                    if (application.enabled) {
                        std::cout << std::setw(12) << std::left << (std::to_string(application.privatePort) + "->" + std::to_string(application.publicPort));
                    }
                    std::cout << std::endl;
                }
                std::cout << "Lambdas: " << std::endl;
                std::cout << "  " << std::setw(32) << std::left << "Name"
                          << std::setw(10) << std::left << "Enabled"
                          << std::setw(10) << std::left << "Status"
                          << std::setw(12) << std::left << "Ports" << std::endl;
                for (const auto &lambda: _lambdas) {
                    std::cout << "  " << std::setw(32) << std::left << lambda.functionName
                              << std::setw(10) << std::left << (lambda.enabled ? "ENABLED" : "DISABLED")
                              << std::setw(10) << std::left << Core::StringUtils::ToUpper(lambda.state);
                    if (lambda.enabled) {
                        const std::vector<Dto::Lambda::InstanceCounter> instances = GetLambdaInstances(lambda);
                        std::string portStr = "8080->";
                        for (const auto &instance: instances) {
                            portStr += std::to_string(instance.publicPort) + ",";
                        }
                        if (!portStr.empty() && portStr.back() == ',') {
                            portStr.pop_back();
                        }
                        std::cout << std::setw(12) << std::left << portStr;
                    }
                    std::cout << std::endl;
                }
                break;
            }

            case CommandType::LIST:
                PrintApplicationList();
                PrintLambdaList();
                break;

            case CommandType::LIST_APPLICATIONS:
                PrintApplicationList();
                break;

            case CommandType::LIST_LAMBDAS:
                PrintLambdaList();
                break;

            case CommandType::ENABLE_APPLICATIONS:
                if (_commands.empty()) {
                    EnableAllApplications();
                } else if (const auto applications = GetApplicationFromCommands(_commands); !applications.empty()) {
                    EnableApplications(applications);
                }
                break;

            case CommandType::ENABLE_LAMBDAS:
                if (_commands.empty()) {
                    EnableAllLambdas();
                } else if (const auto lambdas = GetLambdasFromCommand(_commands); !lambdas.empty()) {
                    EnableLambdas(lambdas);
                }
                break;

            case CommandType::DISABLE_APPLICATIONS:
                if (_commands.empty()) {
                    DisableAllApplications();
                } else if (const auto applications = GetApplicationFromCommands(_commands); !applications.empty()) {
                    DisableApplications(applications);
                }
                break;

            case CommandType::DISABLE_LAMBDAS:
                if (_commands.empty()) {
                    DisableAllLambdas();
                } else if (const auto lambdas = GetLambdasFromCommand(_commands); !lambdas.empty()) {
                    DisableLambdas(lambdas);
                }
                break;

            case CommandType::START_APPLICATIONS:
                if (_commands.empty()) {
                    StartAllApplications();
                } else if (const auto applications = GetApplicationFromCommands(_commands); !applications.empty()) {
                    StartApplications(applications);
                }
                break;

            case CommandType::START_LAMBDAS:
                if (_commands.empty()) {
                    StartAllLambdas();
                } else if (const auto lambdas = GetLambdasFromCommand(_commands); !lambdas.empty()) {
                    StartLambdas(lambdas);
                }
                break;

            case CommandType::RESTART_APPLICATIONS:
                if (_commands.empty()) {
                    RestartAllApplications();
                } else if (const auto applications = GetApplicationFromCommands(_commands); !applications.empty()) {
                    RestartApplications(applications);
                }
                break;

            case CommandType::RESTART_LAMBDAS:
                if (_commands.empty()) {
                    RestartAllLambdas();
                } else if (const auto lambdas = GetLambdasFromCommand(_commands); !lambdas.empty()) {
                    RestartLambdas(lambdas);
                }
                break;

            case CommandType::STOP_APPLICATIONS:
                if (_commands.empty()) {
                    StopAllApplications();
                } else if (const auto applications = GetApplicationFromCommands(_commands); !applications.empty()) {
                    StopApplications(applications);
                }
                break;

            case CommandType::STOP_LAMBDAS:
                if (_commands.empty()) {
                    StopAllLambdas();
                } else if (const auto lambdas = GetLambdasFromCommand(_commands); !lambdas.empty()) {
                    StopLambdas(lambdas);
                }
                break;

            case CommandType::DEPLOY_APPLICATION: {
                if (_commands.size() < 2) {
                    std::cerr << "Missing parameters!" << std::endl;
                    return;
                }
                const std::string applicationName = _commands[0];
                const std::string filename = _commands[1];
                if (applicationName.empty() || filename.empty()) {
                    std::cerr << "Deployment command needs an application name and package filename." << std::endl;
                    return;
                }
                return DeployApplication(applicationName, filename);
            }

            case CommandType::DEPLOY_LAMBDA: {
                if (_commands.size() < 2) {
                    std::cerr << "Missing parameters!" << std::endl;
                    return;
                }
                const std::string functionName = _commands[0];
                const std::string filename = _commands[1];
                if (functionName.empty() || filename.empty()) {
                    std::cerr << "Deployment command needs a lambda function name and package filename." << std::endl;
                    return;
                }
                return DeployLambda(functionName, filename);
            }

            case CommandType::IMPORT:
                ImportInfrastructure();
                break;

            case CommandType::EXPORT: {
                std::vector<Dto::Module::Module> modules = GetModulesFromCommand(_commands);
                if (modules.empty()) {
                    modules = GetAllModules();
                }
                ExportInfrastructure(modules, _vm.contains("pretty"), _vm.contains("include-objects") ? Dto::Module::ExportType::BOTH : Dto::Module::ExportType::INFRA_STRUCTURE);
                break;
            }

            case CommandType::CLEAN: {
                std::vector<Dto::Module::Module> modules = GetModulesFromCommand(_commands);
                if (modules.empty()) {
                    modules = GetAllModules();
                }
                CleanInfrastructure(modules);
                break;
            }

            case CommandType::CLEAN_OBJECTS: {
                std::vector<Dto::Module::Module> modules = GetModulesFromCommand(_commands);
                CleanObjects(modules);
                break;
            }

            case CommandType::PING:
                PingManager();
                break;

#ifdef HAS_SYSTEMD
            case CommandType::LOGS:
                ShowServiceLogs();
                break;
#endif

            default:
                std::cout << "Unknown command: " << action << std::endl;
        }
    }

    void AwsMockCtl::EnableApplications(const std::vector<Dto::Apps::Application> &applications) const {
        for (const auto &application: applications) {
            Dto::Apps::EnableApplicationRequest appRequest;
            appRequest.region = _region;
            appRequest.application.name = application.name;
            appRequest.application.region = application.region;
            if (const auto response = SendCommand("application", "enable-application", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: application: " << application.name << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Application " << application.name << " enabled" << std::endl;
        }
    }

    void AwsMockCtl::EnableAllApplications() const {
        Dto::Apps::EnableAllApplicationsRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("application", "enable-all-applications", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All applications enabled" << std::endl;
    }

    void AwsMockCtl::DisableApplications(const std::vector<Dto::Apps::Application> &applications) const {
        for (const auto &application: applications) {
            Dto::Apps::DisableApplicationRequest appRequest;
            appRequest.region = _region;
            appRequest.application.name = application.name;
            appRequest.application.region = application.region;
            if (const auto response = SendCommand("application", "disable-application", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: application: " << application.name << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Application " << application.name << " disabled" << std::endl;
        }
    }

    void AwsMockCtl::DisableAllApplications() const {
        Dto::Apps::DisableAllApplicationsRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("application", "disable-all-applications", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All applications disabled" << std::endl;
    }

    void AwsMockCtl::StartApplications(const std::vector<Dto::Apps::Application> &applications) const {
        for (const auto &application: applications) {
            Dto::Apps::StartApplicationRequest appRequest;
            appRequest.region = _region;
            appRequest.application.name = application.name;
            appRequest.application.region = application.region;
            if (const auto response = SendCommand("application", "start-application", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: application: " << application.name << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Application " << application.name << " started" << std::endl;
        }
    }

    void AwsMockCtl::StartAllApplications() const {
        Dto::Apps::StartAllApplicationsRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("application", "start-all-applications", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All applications started" << std::endl;
    }

    void AwsMockCtl::RestartApplications(const std::vector<Dto::Apps::Application> &applications) const {
        for (const auto &application: applications) {
            Dto::Apps::RestartApplicationRequest appRequest;
            appRequest.region = _region;
            appRequest.application.name = application.name;
            appRequest.application.region = application.region;
            if (const auto response = SendCommand("application", "restart-application", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: application: " << application.name << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Application " << application.name << " restarted" << std::endl;
        }
    }

    void AwsMockCtl::RestartAllApplications() const {
        if (const auto response = SendCommand("application", "restart-all-applications"); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All applications restarted" << std::endl;
    }

    void AwsMockCtl::StopApplications(const std::vector<Dto::Apps::Application> &applications) const {
        for (const auto &application: applications) {
            Dto::Apps::StopApplicationRequest appRequest;
            appRequest.region = _region;
            appRequest.application.name = application.name;
            appRequest.application.region = application.region;
            if (const auto response = SendCommand("application", "stop-application", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: application: " << application.name << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Application " << application.name << " stopped" << std::endl;
        }
    }

    void AwsMockCtl::StopAllApplications() const {
        Dto::Apps::StopAllApplicationsRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("application", "stop-all-applications", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All applications stopped" << std::endl;
    }

    void AwsMockCtl::EnableLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const {
        for (const auto &lambda: lambdas) {
            Dto::Lambda::EnableLambdaRequest appRequest;
            appRequest.region = _region;
            appRequest.function.functionName = lambda.functionName;
            if (const auto response = SendCommand("lambda", "enable-lambda", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: lambda: " << lambda.functionName << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Lambda " << lambda.functionName << " enabled" << std::endl;
        }
    }

    void AwsMockCtl::EnableAllLambdas() const {
        Dto::Lambda::EnableAllLambdasRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("lambda", "enable-all-lambdas", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All lambdas enabled" << std::endl;
    }

    void AwsMockCtl::DisableLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const {
        for (const auto &lambda: lambdas) {
            Dto::Lambda::DisableLambdaRequest appRequest;
            appRequest.region = _region;
            appRequest.function.functionName = lambda.functionName;
            if (const auto response = SendCommand("lambda", "disable-lambda", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: lambda: " << lambda.functionName << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Lambda " << lambda.functionName << " disabled" << std::endl;
        }
    }

    void AwsMockCtl::DisableAllLambdas() const {
        Dto::Lambda::DisableAllLambdasRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("lambda", "disable-all-lambdas", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All lambdas disabled" << std::endl;
    }

    void AwsMockCtl::StartLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const {
        for (const auto &lambda: lambdas) {
            Dto::Lambda::StartLambdaRequest appRequest;
            appRequest.region = _region;
            appRequest.functionArn = lambda.functionArn;
            if (const auto response = SendCommand("lambda", "start-lambda", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: lambda: " << lambda.functionName << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Lambda function started, name: " << lambda.functionName << std::endl;
        }
    }

    void AwsMockCtl::StartAllLambdas() const {
        Dto::Lambda::StartAllLambasRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("lambda", "start-all-lambdas", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All lambdas started" << std::endl;
    }

    void AwsMockCtl::RestartLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const {
        for (const auto &lambda: lambdas) {
            Dto::Lambda::StartLambdaRequest appRequest;
            appRequest.region = _region;
            appRequest.functionArn = lambda.functionArn;
            if (const auto response = SendCommand("lambda", "restart-lambda", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: lambda: " << lambda.functionName << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Lambda function restarted, name: " << lambda.functionName << std::endl;
        }
    }

    void AwsMockCtl::RestartAllLambdas() const {
        Dto::Lambda::StartAllLambasRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("lambda", "restart-all-lambdas", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All lambdas restarted" << std::endl;
    }

    void AwsMockCtl::StopLambdas(const std::vector<Dto::Lambda::Function> &lambdas) const {
        for (const auto &lambda: lambdas) {
            Dto::Lambda::StopLambdaRequest appRequest;
            appRequest.region = _region;
            appRequest.functionArn = lambda.functionArn;
            if (const auto response = SendCommand("lambda", "stop-lambda", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
                std::cerr << "Error: lambda: " << lambda.functionName << ", httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
                return;
            }
            std::cout << "Lambda function stopped, name: " << lambda.functionName << std::endl;
        }
    }

    void AwsMockCtl::StopAllLambdas() const {
        Dto::Lambda::StopAllLambasRequest appRequest;
        appRequest.region = _region;
        if (const auto response = SendCommand("lambda", "stop-all-lambdas", appRequest.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: httpStatus: " << response.statusCode << ", body: " << response.body << std::endl;
            return;
        }
        std::cout << "All lambdas stopped" << std::endl;
    }

    void AwsMockCtl::DeployApplication(const std::string &applicationName, const std::string &filename) const {
        if (!Core::FileUtils::FileExists(filename)) {
            std::cerr << "Error: Application package not found" << std::endl;
            return;
        }
        Dto::Apps::UploadApplicationCodeRequest request;
        request.region = _region;
        request.user = _user;
        request.requestId = Core::AwsUtils::CreateRequestId();
        request.applicationName = applicationName;
        request.version = Core::FileUtils::ExtractVersionFromFileName(filename);
        request.applicationCode = Core::Crypto::Base64Encode(Core::FileUtils::ReadFile(filename));
        request.archive = Core::FileUtils::StripBasePath(filename);
        if (const auto response = SendCommand("application", "upload-application", request.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: '" << response.statusCode << "', body: " << response.body << std::endl;
            return;
        }
        std::cout << "Application deployed, name: " << applicationName << ", file: " << filename << std::endl;
    }

    void AwsMockCtl::DeployLambda(const std::string &functionName, const std::string &filename) const {
        if (!Core::FileUtils::FileExists(filename)) {
            std::cerr << "Error: Lambda function package not found" << std::endl;
            return;
        }
        Dto::Lambda::UploadFunctionCodeRequest request;
        request.region = _region;
        request.user = _user;
        request.requestId = Core::AwsUtils::CreateRequestId();
        request.functionArn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, functionName);
        request.version = Core::FileUtils::ExtractVersionFromFileName(filename);
        request.functionCode = Core::Crypto::Base64Encode(Core::FileUtils::ReadFile(filename));
        if (const auto response = SendCommand("lambda", "upload-function-code", request.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Error: '" << response.statusCode << "', body: " << response.body << std::endl;
            return;
        }
        std::cout << "Lambda function deployed, name: " << functionName << ", file: " << filename << std::endl;
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

    void AwsMockCtl::GetLogLevel() const {
        const auto response = SendGetCommand("module", "get-log-level");
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get log level, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }
        std::cout << "Log level is: " << response.body << std::endl;
    }

    void AwsMockCtl::SetLogLevel(const std::string &level, const std::string &channel) const {

        if (!VerifyLogLevel(level)) {
            std::cerr << "Invalid log level: " << level << std::endl;
            return;
        }

        if (!channel.empty() && !VerifyChannel(channel)) {
            std::cerr << "Invalid channel: " << channel << std::endl;
            return;
        }

        Dto::Module::SetLogLevelRequest request;
        request.level = level;
        request.channel = channel;
        if (const auto response = SendCommand("module", "set-log-level", request.ToJson()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not set log level, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }
        if (channel.empty()) {
            std::cout << "Log level set to " << level << std::endl;
        } else {
            std::cout << "Log level for channel " << channel << " set to " << level << std::endl;
        }
    }

    void AwsMockCtl::PingManager() const {
        const auto start = system_clock::now();
        if (const auto response = SendGetCommand("module", "ping"); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "No response from manager, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }
        const long duration = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();
        std::cout << "Manager pinged (" << duration << "ms)" << std::endl;
    }

    void AwsMockCtl::GetConfig() const {
        const auto response = SendGetCommand("module", "get-config");
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
        std::cout << "  " << std::setw(16) << std::left << "Database: " << std::setw(32) << (gatewayConfig.databaseActive ? "true" : "false") << std::endl;
    }

    void AwsMockCtl::ExportInfrastructure(const std::vector<Dto::Module::Module> &modules, const bool pretty, const Dto::Module::ExportType &exportType) const {
        Dto::Module::ExportInfrastructureRequest moduleRequest;
        for (const auto &module: modules) {
            moduleRequest.modules.emplace_back(module.name);
        }
        moduleRequest.exportType = exportType;
        moduleRequest.prettyPrint = pretty;
        const auto response = SendCommand("module", "export", moduleRequest.ToJson());
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
        if (const auto response = SendCommand("module", "import", jsonString.str()); response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not import objects, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
        }
    }

    void AwsMockCtl::CleanInfrastructure(const Dto::Module::Module::ModuleList &modules) const {
        const auto response = SendGetCommand("module", "clean", Dto::Module::Module::ToJson(modules));
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not clean infrastructure, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return;
        }
        std::cout << response.body;
    }

    void AwsMockCtl::CleanObjects(Dto::Module::Module::ModuleList &modules) const {
        std::map<std::string, std::string> headers;
        AddStandardHeaders(headers, "module", "clean-objects");
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

    void AwsMockCtl::AddStandardHeaders(std::map<std::string, std::string> &headers, const std::string &target, const std::string &action) const {
        headers["User"] = _user;
        headers["Region"] = _region;
        headers["x-awsmock-target"] = target;
        headers["x-awsmock-action"] = action;
    }

    std::vector<Dto::Apps::Application> AwsMockCtl::GetAllApplications() const {
        Dto::Apps::ListApplicationCountersRequest request;
        request.region = _region;
        request.user = _user;
        const auto response = SendCommand("application", "list-applications", request.ToJson());
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get application list, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return {};
        }
        std::vector<Dto::Apps::Application> applications;
        if (auto const jv = boost::json::parse(response.body); !jv.as_object().empty()) {
            for (boost::json::array appArray = jv.at("applications").as_array(); const auto &element: appArray) {
                Dto::Apps::Application application;
                application.name = element.at("name").as_string().data();
                application.status = Dto::Apps::AppsStatusTypeFromString(element.at("status").as_string().data());
                application.enabled = element.at("enabled").as_bool();
                application.publicPort = element.at("publicPort").as_int64();
                application.privatePort = element.at("privatePort").as_int64();
                applications.emplace_back(application);
            }
        }
        return applications;
    }

    std::vector<Dto::Lambda::Function> AwsMockCtl::GetAllLambdas() const {
        Dto::Lambda::ListFunctionCountersRequest request;
        request.region = _region;
        request.user = _user;
        const auto response = SendCommand("lambda", "list-function-counters", request.ToJson());
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get lambda list, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return {};
        }
        std::vector<Dto::Lambda::Function> lambdas;
        if (auto const jv = boost::json::parse(response.body); !jv.as_object().empty()) {
            for (boost::json::array lambdaArray = jv.at("functionCounters").as_array(); const auto &element: lambdaArray) {
                Dto::Lambda::Function lambda;
                lambda.functionName = element.at("functionName").as_string().data();
                lambda.state = element.at("state").as_string().data();
                lambda.enabled = element.at("enabled").as_bool();
                lambda.functionArn = element.at("functionArn").as_string().data();
                lambdas.push_back(lambda);
            }
        }
        return lambdas;
    }

    std::vector<Dto::Lambda::InstanceCounter> AwsMockCtl::GetLambdaInstances(const Dto::Lambda::Function &function) const {
        Dto::Lambda::ListLambdaInstanceCountersRequest request;
        request.region = _region;
        request.user = _user;
        request.lambdaArn = function.functionArn;
        const auto response = SendCommand("lambda", "list-instance-counters", request.ToJson());
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get lambda instance list, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return {};
        }
        std::vector<Dto::Lambda::InstanceCounter> instances;
        if (auto const jv = boost::json::parse(response.body); !jv.as_object().empty()) {
            for (boost::json::array instanceArray = jv.at("instanceCounters").as_array(); const auto &element: instanceArray) {
                Dto::Lambda::InstanceCounter instance;
                instance.containerId = element.at("containerId").as_string().data();
                instance.instanceId = element.at("instanceId").as_string().data();
                instance.publicPort = element.at("publicPort").as_int64();
                instance.privatePort = element.at("privatePort").as_int64();
                instances.push_back(instance);
            }
        }
        return instances;
    }

    std::vector<Dto::Module::Module> AwsMockCtl::GetAllModules() const {
        const auto response = SendGetCommand("module", "list-modules");
        if (response.statusCode != boost::beast::http::status::ok) {
            std::cerr << "Could not get module list, httpStatus: " << response.statusCode << " body:" << response.body << std::endl;
            return {};
        }
        std::vector<Dto::Module::Module> modules;
        if (auto const jv = boost::json::parse(response.body); !jv.as_object().empty()) {
            for (boost::json::array appArray = jv.at("modules").as_array(); const auto &element: appArray) {
                Dto::Module::Module module;
                module.name = element.at("name").as_string().data();
                modules.push_back(module);
            }
        }
        return modules;
    }

    std::vector<Dto::Apps::Application> AwsMockCtl::GetApplicationFromCommands(const std::vector<std::string> &commands) {
        std::vector<Dto::Apps::Application> applications;
        std::ranges::copy_if(_applications, std::back_inserter(applications), [&](const auto &a) {
            return std::ranges::contains(commands, a.name);
        });
        return applications;
    }

    std::vector<Dto::Lambda::Function> AwsMockCtl::GetLambdasFromCommand(const std::vector<std::string> &commands) {
        std::vector<Dto::Lambda::Function> lambdas;
        std::ranges::copy_if(_lambdas, std::back_inserter(lambdas), [&](const auto &f) {
            return std::ranges::contains(commands, f.functionName);
        });
        return lambdas;
    }

    std::vector<Dto::Module::Module> AwsMockCtl::GetModulesFromCommand(const std::vector<std::string> &commands) {
        std::vector<Dto::Module::Module> modules;
        for (const auto &command: commands) {
            if (std::ranges::find(_lambdas, command, &Dto::Lambda::Function::functionName) == _lambdas.end()) {
                Dto::Module::Module module;
                module.name = command;
                modules.push_back(module);
            }
        }
        return modules;
    }

    bool AwsMockCtl::VerifyLogLevel(const std::string &level) {
        return std::ranges::find(validLogLevels.begin(), validLogLevels.end(), level) != validLogLevels.end();
    }

    bool AwsMockCtl::VerifyChannel(const std::string &channel) {
        return std::ranges::find(validChannels.begin(), validChannels.end(), channel) != validChannels.end();
    }
}// namespace AwsMock::Controller
