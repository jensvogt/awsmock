//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/module/ModuleHandler.h>

namespace Awsmock::Service {

    http::response<http::dynamic_body> ModuleHandler::HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        Monitoring::MonitoringTimer measure(MODULE_HTTP_TIMER, MODULE_HTTP_COUNTER, "method", "GET");

        std::string target = request.base()["x-awsmock-target"];
        std::string action = request.base()["x-awsmock-action"];
        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        log_debug << "Found action and target, target: " << target << " action: " << action;

        try {
            if (action == "get-config") {
                log_info << "Get config request";

                Core::Configuration &configuration = Core::Configuration::instance();
                auto host = configuration.get<std::string>("awsmock.gateway.http.host");
                auto address = configuration.get<std::string>("awsmock.gateway.http.address");
                int port = configuration.get<int>("awsmock.gateway.http.port");
                std::string endpoint = "http://" + host + ":" + std::to_string(port);
                Dto::Module::GatewayConfig config;
                config.endpoint = endpoint;
                config.host = host;
                config.address = address;
                config.port = port;
                config.pid = Core::SystemUtils::GetPid();
                config.user = configuration.get<std::string>("awsmock.user");
                config.accessId = configuration.get<std::string>("awsmock.access.account-id");
                config.clientId = configuration.get<std::string>("awsmock.access.client-id");
                config.dataDir = configuration.get<std::string>("awsmock.data-dir");
                config.version = Core::Configuration::getVersion();
                config.databaseActive = configuration.get<bool>("awsmock.mongodb.active");
                return SendResponse(request, http::status::ok, config.ToJson());
            }

            if (action == "list-modules") {

                log_info << "List modules";
                std::vector<Database::Entity::Module::Module> modules = _moduleService.ListModules();
                std::string body = Dto::Module::Module::ToJson(modules);
                return SendResponse(request, http::status::ok, body);
            }

            if (action == "list-module-names") {

                log_info << "List module names";
                Dto::Module::ListModuleNamesResponse modulesResponse = _moduleService.ListModuleNames();
                return SendResponse(request, http::status::ok, modulesResponse.ToJson());
            }

            if (action == "get-infrastructure") {

                // Get request body
                log_info << "Get infrastructure";
                Dto::Module::ExportInfrastructureRequest moduleRequest = Dto::Module::ExportInfrastructureRequest::FromJson(payload);

                // Get modules
                const Dto::Module::ExportInfrastructureResponse moduleResponse = _moduleService.ExportInfrastructure(moduleRequest);
                std::string json = moduleResponse.ToJson();
                log_info << "Infrastructure send to client, size: " << json.length();
                return SendResponse(request, http::status::ok, json);
            }

            if (action == "ping") {
                return SendResponse(request, http::status::ok);
            }

            log_error << "Unknown action, target: " << target << ", action: " << action << ", method: GET";
            return SendResponse(request, http::status::bad_request, "Unknown action");

        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        }
    }

    http::response<http::dynamic_body> ModuleHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        Monitoring::MonitoringTimer measure(MODULE_HTTP_TIMER, MODULE_HTTP_COUNTER, "method", "POST");

        try {

            const std::string target = request.base()["x-awsmock-target"];
            const std::string action = request.base()["x-awsmock-action"];
            const std::string payload = Core::HttpUtils::GetBodyAsString(request);
            log_debug << "Found action and target, target: " << target << " action: " << action;

            if (action == "start-modules") {

                Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
                modules = _moduleService.StartModules(modules);
                return SendResponse(request, http::status::ok, Dto::Module::Module::ToJson(modules));
            }

            if (action == "restart-modules") {

                Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
                modules = _moduleService.StopModules(modules);
                modules = _moduleService.StartModules(modules);
                return SendResponse(request, http::status::ok, Dto::Module::Module::ToJson(modules));
            }
            if (action == "stop-modules") {

                Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
                modules = _moduleService.StopModules(modules);
                return SendResponse(request, http::status::ok, Dto::Module::Module::ToJson(modules));
            }
            if (action == "import") {

                Dto::Module::ImportInfrastructureRequest infrastructureRequest;
                infrastructureRequest.FromJson(payload);
                ModuleService{}.ImportInfrastructure(infrastructureRequest);
                log_info << "Infrastructure imported, size: " << payload.length();
                return SendResponse(request, http::status::ok);
            }

            if (action == "export") {

                // Get request body
                Dto::Module::ExportInfrastructureRequest moduleRequest = Dto::Module::ExportInfrastructureRequest::FromJson(payload);

                // Get modules
                const Dto::Module::ExportInfrastructureResponse moduleResponse = ModuleService{}.ExportInfrastructure(moduleRequest);
                std::string json = moduleResponse.ToJson();
                log_info << "Infrastructure exported, size: " << json.length();
                return SendResponse(request, http::status::ok, json);
            }

            if (action == "set-log-level") {

                const auto moduleRequest = Dto::Module::SetLogLevelRequest::FromJson(payload);
                _moduleService.setLogLevel(moduleRequest);
                return SendResponse(request, http::status::ok);
            }

            if (action == "get-log-level") {

                Dto::Module::GetLogLevelRequest moduleRequest = Dto::Module::GetLogLevelRequest::FromJson(payload);
                Dto::Module::GetLogLevelResponse moduleResponse = _moduleService.getLogLevels(moduleRequest);
                log_info << "Current log levels, size: " << moduleResponse.contentType.size();

                // Send response
                return SendResponse(request, http::status::ok, moduleResponse.ToJson());
            }

            if (action == "clean-objects") {

                // Get clean modules request
                if (Dto::Module::CleanInfrastructureRequest moduleRequest = Dto::Module::CleanInfrastructureRequest::FromJson(payload); moduleRequest.onlyObjects) {
                    _moduleService.CleanObjects(moduleRequest);
                } else {
                    _moduleService.CleanInfrastructure(moduleRequest);
                }
                return SendResponse(request, http::status::ok);
            }

            log_error << "Unknown action, target: " << target << ", action: " << action << ", method: POST";
            return SendResponse(request, http::status::bad_request, "Unknown action");

        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        }
    }

}// namespace Awsmock::Service
