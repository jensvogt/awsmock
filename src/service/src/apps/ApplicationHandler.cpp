

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
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.CreateApplication(serviceRequest);
                    log_info << "Application created, name: " << serviceRequest.application.name;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::GET_APPLICATION: {

                    Dto::Apps::GetApplicationRequest serviceRequest = Dto::Apps::GetApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::GetApplicationResponse serviceResponse = _applicationService.GetApplication(serviceRequest);
                    log_info << "Application retrieved, name: " << serviceRequest.name;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::UPDATE_APPLICATION: {

                    Dto::Apps::UpdateApplicationRequest serviceRequest = Dto::Apps::UpdateApplicationRequest::FromJson(clientCommand);
                    serviceRequest.application.region = region;
                    Dto::Apps::GetApplicationResponse serviceResponse = _applicationService.UpdateApplication(serviceRequest);
                    log_info << "Application updated, name: " << serviceRequest.application.name;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::UPLOAD_APPLICATION: {

                    Dto::Apps::UploadApplicationCodeRequest serviceRequest = Dto::Apps::UploadApplicationCodeRequest::FromJson(clientCommand);
                    _applicationService.UploadApplicationCode(serviceRequest);
                    log_info << "Upload application code, name: " << serviceRequest.applicationName;
                    return SendOkResponse(request);
                }

                case Dto::Common::ApplicationCommandType::LIST_APPLICATIONS: {

                    Dto::Apps::ListApplicationCountersRequest serviceRequest = Dto::Apps::ListApplicationCountersRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.ListApplications(serviceRequest);
                    log_info << "Applications listed, region: " << serviceRequest.region;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::START_APPLICATION: {

                    Dto::Apps::StartApplicationRequest serviceRequest = Dto::Apps::StartApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.StartApplication(serviceRequest);
                    log_info << "Applications started, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::STOP_APPLICATION: {

                    Dto::Apps::StopApplicationRequest serviceRequest = Dto::Apps::StopApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.StopApplication(serviceRequest);
                    log_info << "Applications stopped, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    return SendOkResponse(request, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::DELETE_APPLICATION: {

                    Dto::Apps::DeleteApplicationRequest serviceRequest = Dto::Apps::DeleteApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.DeleteApplication(serviceRequest);
                    log_info << "Application deleted, name: " << serviceRequest.name;
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
