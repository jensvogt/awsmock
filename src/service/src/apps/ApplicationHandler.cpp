
#include <awsmock/service/apps/ApplicationHandler.h>

namespace AwsMock::Service {

    const std::map<std::string, std::string> ApplicationHandler::headers = CreateHeaderMap();

    http::response<http::dynamic_body> ApplicationHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Application POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ApplicationClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            switch (clientCommand.command) {
                case Dto::Common::ApplicationCommandType::CREATE_APPLICATION: {

                    Dto::Apps::CreateApplicationRequest serviceRequest = Dto::Apps::CreateApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::CreateApplicationResponse serviceResponse = _applicationService.CreateApplication(serviceRequest);
                    log_info << "Application created, name: " << serviceResponse.application.name;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::LIST_APPLICATIONS: {

                    Dto::Apps::ListApplicationCountersRequest serviceRequest = Dto::Apps::ListApplicationCountersRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.ListApplications(serviceRequest);
                    log_info << "Applications listed, region: " << serviceRequest.region;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                default:
                    log_error << "Unknown action";
                    return SendBadRequestError(request, "Unknown action");
            }
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        }
    }

}// namespace AwsMock::Service
