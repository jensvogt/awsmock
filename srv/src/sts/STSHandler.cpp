//
// Created by vogje01 on 19/07/2026.
//

#include <awsmock/service/sts/STSHandler.h>

namespace Awsmock::Service {

    http::response<http::dynamic_body> STSHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "STS POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::STSClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            switch (clientCommand.command) {

                case Dto::Common::STSCommandType::ASSUME_ROLE: {
                    auto stsRequest = Dto::STS::AssumeRoleRequest::FromClientCommand(clientCommand);
                    auto stsResponse = _stsService.AssumeRole(stsRequest);
                    log_info << "AssumeRole, roleArn: " << stsRequest.roleArn;
                    std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                    return SendResponse(request, http::status::ok, stsResponse.ToXml(), headers);
                }

                case Dto::Common::STSCommandType::GET_CALLER_IDENTITY: {
                    auto stsResponse = _stsService.GetCallerIdentity(region);
                    log_info << "GetCallerIdentity, region: " << region;
                    std::map<std::string, std::string> headers{{"Content-Type", "text/xml"}};
                    return SendResponse(request, http::status::ok, stsResponse.ToXml(), headers);
                }

                default:
                case Dto::Common::STSCommandType::UNKNOWN: {
                    log_error << "Unknown STS command, payload: " << clientCommand.payload;
                    throw Core::ServiceException("Unknown STS command");
                }
            }

        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

}// namespace Awsmock::Service
