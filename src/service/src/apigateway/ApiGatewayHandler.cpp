//
// Created by vogje01 on 9/2/25.
//

#include <awsmock/service/apigateway/ApiGatewayHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> ApiGatewayHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Application POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ApiGatewayClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        Core::HttpUtils::DumpRequest(request);

        try {
            switch (clientCommand.command) {

                case Dto::Common::ApiGatewayCommandType::CREATE_API_KEY: {

                    Dto::ApiGateway::CreateApiKeyRequest serviceRequest = Dto::ApiGateway::CreateApiKeyRequest::FromJson(clientCommand);
                    const Dto::ApiGateway::CreateApiKeyResponse serviceResponse = _apiGatewayService.CreateApiKey(serviceRequest);
                    log_info << "API key created, name: " << serviceRequest.name;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                default:
                    log_error << "Unknown action";
                    return SendResponse(request, http::status::bad_request, "Unknown action");
            }
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, "Unknown action");
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

}// namespace AwsMock::Service