//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/module/ModuleHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> ModuleHandler::HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        Monitoring::MetricServiceTimer measure(MODULE_HTTP_TIMER, "method", "GET");
        Monitoring::MetricService::instance().IncrementCounter(MODULE_HTTP_COUNTER, "method", "GET");

        std::string target = request.base()["x-awsmock-target"];
        std::string action = request.base()["x-awsmock-action"];
        std::string payload = Core::HttpUtils::GetBodyAsString(request);
        log_debug << "Found action and target, target: " << target << " action: " << action;

        try {
            if (action == "get-config") {

                Core::Configuration &configuration = Core::Configuration::instance();
                auto host = configuration.GetValue<std::string>("awsmock.gateway.http.host");
                auto address = configuration.GetValue<std::string>("awsmock.gateway.http.address");
                int port = configuration.GetValue<int>("awsmock.gateway.http.port");
                std::string endpoint = "http://" + host + ":" + std::to_string(port);
                Dto::Module::GatewayConfig config = {
                        .region = configuration.GetValue<std::string>("awsmock.region"),
                        .endpoint = endpoint,
                        .host = host,
                        .address = address,
                        .port = port,
#ifndef WIN32
                        .pid = getppid(),
#endif
                        .user = configuration.GetValue<std::string>("awsmock.user"),
                        .accessId = configuration.GetValue<std::string>("awsmock.access.account-id"),
                        .clientId = configuration.GetValue<std::string>("awsmock.access.client-id"),
                        .dataDir = configuration.GetValue<std::string>("awsmock.data-dir"),
                        .version = configuration.GetVersion(),
                        .databaseActive = configuration.GetValue<bool>("awsmock.mongodb.active")};
                return SendOkResponse(request, config.ToJson());
            }
            if (action == "list-modules") {

                Database::Entity::Module::ModuleList modules = _moduleService.ListModules();
                std::string body = Dto::Module::Module::ToJson(modules);
                return SendOkResponse(request, body);
            }
            if (action == "list-module-names") {

                Dto::Module::ListModuleNamesResponse modulesResponse = _moduleService.ListModuleNames();
                return SendOkResponse(request, modulesResponse.ToJson());
            }
            if (action == "show-ftp-users") {

                Dto::Transfer::Server server;
                server.FromJson(payload);

                Dto::Transfer::ListUsersRequest transferRequest = {.region = region, .serverId = server.serverId};
                TransferService transferService;
                Dto::Transfer::ListUsersResponse transferResponse = transferService.ListUsers(transferRequest);
                return SendOkResponse(request, transferResponse.ToJson());
            }
            if (action == "ping") {

                return SendOkResponse(request);
            }
            return SendBadRequestError(request, "Unknown action");

        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        }
    }

    http::response<http::dynamic_body> ModuleHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        Monitoring::MetricServiceTimer measure(MODULE_HTTP_TIMER, "method", "POST");
        Monitoring::MetricService::instance().IncrementCounter(MODULE_HTTP_COUNTER, "method", "POST");

        try {

            const std::string target = request.base()["x-awsmock-target"];
            const std::string action = request.base()["x-awsmock-action"];
            const std::string payload = Core::HttpUtils::GetBodyAsString(request);
            log_debug << "Found action and target, target: " << target << " action: " << action;

            if (action == "start-modules") {

                Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
                modules = _moduleService.StartModules(modules);
                return SendOkResponse(request, Dto::Module::Module::ToJson(modules));
            }
            if (action == "restart-modules") {

                Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
                modules = _moduleService.StopModules(modules);
                modules = _moduleService.StartModules(modules);
                return SendOkResponse(request, Dto::Module::Module::ToJson(modules));
            }
            if (action == "stop-modules") {

                Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
                modules = _moduleService.StopModules(modules);
                return SendOkResponse(request, Dto::Module::Module::ToJson(modules));
            }
            if (action == "import") {

                Dto::Module::ImportInfrastructureRequest moduleRequest;
                moduleRequest.FromJson(payload);
                _moduleService.ImportInfrastructure(moduleRequest);
                return SendOkResponse(request);
            }
            if (action == "set-log-level") {

                Core::LogStream::SetSeverity(payload);
                log_info << "Log level set to '" << payload << "'";

                // Send response
                return SendOkResponse(request);
            }
            if (action == "clean-objects") {

                // Get request body
                Dto::Module::CleanInfrastructureRequest moduleRequest;
                moduleRequest.FromJson(payload);

                // Get modules
                if (moduleRequest.onlyObjects) {
                    ModuleService::CleanObjects(moduleRequest);
                } else {
                    ModuleService::CleanInfrastructure(moduleRequest);
                }
                return SendOkResponse(request);
            }
            if (action == "export") {

                // Get request body
                Dto::Module::ExportInfrastructureRequest moduleRequest;
                moduleRequest.FromJson(payload);

                // Get modules
                Dto::Module::ExportInfrastructureResponse moduleResponse = _moduleService.ExportInfrastructure(moduleRequest);
                if (moduleResponse.ToJson().length() > 100000000) {
                    log_error << "Response > 10MB";
                    return SendBadRequestError(request, "Size > 100 MB.");
                }
                return SendOkResponse(request, moduleResponse.ToJson());
            }
            return SendBadRequestError(request, "Unknown action");
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        }
    }

}// namespace AwsMock::Service