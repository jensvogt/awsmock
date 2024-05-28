//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/server/Handler.h>

namespace AwsMock::Manager {

    boost::beast::http::response<boost::beast::http::string_body> Handler::HandleGetRequest(boost::beast::http::request<boost::beast::http::string_body> &request) {

        Core::Configuration &configuration = Core::Configuration::instance();
        Core::MetricServiceTimer measure(MODULE_HTTP_TIMER, "GET");
        Core::MetricService::instance().IncrementCounter(MODULE_HTTP_COUNTER, "method", "GET");

        std::string target = request.base()["Target"];
        std::string action = request.base()["Action"];
        std::string payload = request.body();
        log_debug << "Found action and target, target: " << target << " action: " << action;

        if (action == "get-config") {

            std::string host = configuration.getString("awsmock.service.gateway.http.host", "localhost");
            std::string address = configuration.getString("awsmock.service.gateway.http.address", "0.0.0.0");
            int port = configuration.getInt("awsmock.service.gateway.http.port", 4566);
            std::string endpoint = "http://" + host + ":" + std::to_string(port);
            Dto::Module::GatewayConfig config = {
                    .region = configuration.getString("awsmock.region", "eu-central-1"),
                    .endpoint = endpoint,
                    .host = host,
                    .address = address,
                    .port = port,
                    .pid = getppid(),
                    .user = configuration.getString("awsmock.user", "none"),
                    .accessId = configuration.getString("awsmock.account.userPoolId", "000000000000"),
                    .clientId = configuration.getString("awsmock.client.userPoolId", "00000000"),
                    .dataDir = configuration.getString("awsmock.data.dir", "/tmp/awsmock/data"),
                    .version = Core::Configuration::GetVersion(),
                    .databaseActive = configuration.getBool("awsmock.mongodb.active", false)};
            return SendOkResponse(request, config.ToJson());

        } else if (action == "list-services") {

            Database::Entity::Module::ModuleList modules = _moduleService.ListModules();
            std::string body = Dto::Module::Module::ToJson(modules);
            return SendOkResponse(request, body);

        } else if (action == "export") {

            bool prettyPrint = Core::HttpUtils::GetQueryParameterValueByName(request.target(), "pretty") == "true";
            bool includeObjects = Core::HttpUtils::GetQueryParameterValueByName(request.target(), "includeObjects") == "true";

            Dto::Common::Services services;
            services.FromJson(payload);
            std::string infrastructureJson = AwsMock::Service::ModuleService::ExportInfrastructure(services, prettyPrint, includeObjects);
            return SendOkResponse(request, infrastructureJson);

        } else if (action == "clean") {

            Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
            AwsMock::Service::ModuleService::CleanInfrastructure(modules);
            return SendOkResponse(request);

        } else {

            // Returns a not found response
            auto const not_found =
                    [&request](boost::beast::string_view target) {
                        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, request.version()};
                        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(boost::beast::http::field::content_type, "text/html");
                        res.keep_alive(request.keep_alive());
                        res.body() = "The resource '" + std::string(target) + "' was not found.";
                        res.prepare_payload();
                        return res;
                    };
            return not_found("Action unknown");
        }
    }

    boost::beast::http::response<boost::beast::http::string_body> Handler::HandlePutRequest(boost::beast::http::request<boost::beast::http::string_body> &request) {

        Core::MetricServiceTimer measure(MODULE_HTTP_TIMER, "PUT");
        Core::MetricService::instance().IncrementCounter(MODULE_HTTP_COUNTER, "method", "PUT");

        std::string target = request.base()["Target"];
        std::string action = request.base()["Action"];
        std::string payload = request.body();
        log_debug << "Found action and target, target: " << target << " action: " << action;

        if (action == "start-modules") {

            Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
            modules = _moduleService.StartModules(modules);
            return SendOkResponse(request, Dto::Module::Module::ToJson(modules));

        } else if (action == "restart-modules") {

            Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
            modules = _moduleService.StopModules(modules);
            modules = _moduleService.StartModules(modules);
            return SendOkResponse(request, Dto::Module::Module::ToJson(modules));

        } else if (action == "stop-modules") {

            Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
            modules = _moduleService.StopModules(modules);
            return SendOkResponse(request, Dto::Module::Module::ToJson(modules));

        } else if (action == "import") {

            _moduleService.ImportInfrastructure(payload);
            return SendOkResponse(request);

        } else if (action == "set-log-level") {

            plog::get()->setMaxSeverity(plog::severityFromString(payload.c_str()));
            log_info << "Log level set to '" << payload << "'";

            // Send response
            return SendOkResponse(request);

        } else if (action == "clean-objects") {

            Dto::Module::Module::ModuleList modules = Dto::Module::Module::FromJsonList(payload);
            _moduleService.CleanObjects(modules);
            return SendOkResponse(request, Dto::Module::Module::ToJson(modules));

        } else {
            // Returns a not found response
            auto const not_found =
                    [&request](boost::beast::string_view target) {
                        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, request.version()};
                        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(boost::beast::http::field::content_type, "text/html");
                        res.keep_alive(request.keep_alive());
                        res.body() = "The resource '" + std::string(target) + "' was not found.";
                        res.prepare_payload();
                        return res;
                    };
            return not_found("Action unknown");
        }
    }

    boost::beast::http::response<boost::beast::http::string_body> Handler::HandlePostRequest(boost::beast::http::request<boost::beast::http::string_body> &request) {
        return {};
    }

    boost::beast::http::response<boost::beast::http::string_body> Handler::SendOkResponse(boost::beast::http::request<boost::beast::http::string_body> &request, const std::string &body) {
        // Prepare the response message
        boost::beast::http::response<boost::beast::http::string_body> response;
        response.version(request.version());
        response.result(boost::beast::http::status::ok);
        response.set(boost::beast::http::field::server, "awsmock");
        response.set(boost::beast::http::field::content_type, "application/json");
        response.body() = body;
        response.prepare_payload();

        // Send the response to the client
        return response;
    }

}// namespace AwsMock::Manager