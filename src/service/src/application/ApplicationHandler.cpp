

#include <awsmock/service/apps/ApplicationHandler.h>

namespace AwsMock::Service {

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
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::GET_APPLICATION: {

                    Dto::Apps::GetApplicationRequest serviceRequest = Dto::Apps::GetApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::GetApplicationResponse serviceResponse = _applicationService.GetApplication(serviceRequest);
                    log_info << "Application retrieved, name: " << serviceRequest.name;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::UPDATE_APPLICATION: {

                    Dto::Apps::UpdateApplicationRequest serviceRequest = Dto::Apps::UpdateApplicationRequest::FromJson(clientCommand);
                    serviceRequest.application.region = region;
                    Dto::Apps::GetApplicationResponse serviceResponse = _applicationService.UpdateApplication(serviceRequest);
                    log_info << "Application updated, name: " << serviceRequest.application.name;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::UPLOAD_APPLICATION: {

                    Dto::Apps::UploadApplicationCodeRequest serviceRequest = Dto::Apps::UploadApplicationCodeRequest::FromJson(clientCommand);
                    _applicationService.UploadApplicationCode(serviceRequest);
                    log_info << "Upload application code succeeded, name: " << serviceRequest.applicationName << ", version: " << serviceRequest.version;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::LIST_APPLICATIONS: {

                    Dto::Apps::ListApplicationCountersRequest serviceRequest = Dto::Apps::ListApplicationCountersRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.ListApplications(serviceRequest);
                    log_info << "Applications listed, region: " << serviceRequest.region;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::LIST_APPLICATION_NAMES: {

                    Dto::Apps::ListApplicationCountersRequest serviceRequest = Dto::Apps::ListApplicationCountersRequest::FromJson(clientCommand);
                    std::vector<std::string> serviceResponse = _applicationService.ListApplicationNames();
                    log_info << "Application names listed, region: " << serviceRequest.region;
                    return SendResponse(request, http::status::ok, boost::json::serialize(serviceResponse));
                }

                case Dto::Common::ApplicationCommandType::REBUILD_APPLICATION: {

                    Dto::Apps::RebuildApplicationCodeRequest serviceRequest = Dto::Apps::RebuildApplicationCodeRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.RebuildApplication(serviceRequest);
                        log_info << "Applications rebuild, region: " << serviceRequest.region; });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::ENABLE_APPLICATION: {

                    Dto::Apps::EnableApplicationRequest serviceRequest = Dto::Apps::EnableApplicationRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.EnableApplication(serviceRequest);
                        log_info << "Application enabled, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name; });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::ENABLE_ALL_APPLICATIONS: {

                    Dto::Apps::EnableAllApplicationsRequest serviceRequest = Dto::Apps::EnableAllApplicationsRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.EnableAllApplications(serviceRequest);
                        log_info << "Application enabled, region: " << serviceRequest.region; });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::DISABLE_APPLICATION: {

                    Dto::Apps::DisableApplicationRequest serviceRequest = Dto::Apps::DisableApplicationRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.DisableApplication(serviceRequest);
                        log_info << "Application disabled, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name; });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::DISABLE_ALL_APPLICATIONS: {

                    Dto::Apps::DisableAllApplicationsRequest serviceRequest = Dto::Apps::DisableAllApplicationsRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.DisableAllApplications(serviceRequest);
                        log_info << "Application disabled, region: " << serviceRequest.region; });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::START_APPLICATION: {

                    Dto::Apps::StartApplicationRequest serviceRequest = Dto::Apps::StartApplicationRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.StartApplication(serviceRequest);
                        log_info << "Application started, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::START_ALL_APPLICATIONS: {

                    boost::asio::post(_ioc, [this] {
                        const ApplicationService service{_ioc};
                        const long count = service.StartAllApplications();
                        log_info << "All applications started, count: " << count; });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::RESTART_APPLICATION: {

                    Dto::Apps::RestartApplicationRequest serviceRequest = Dto::Apps::RestartApplicationRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.RestartApplication(serviceRequest);
                        log_info << "Applications restarted, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name; });
                    return SendOkResponse(request);
                }

                case Dto::Common::ApplicationCommandType::RESTART_ALL_APPLICATIONS: {

                    boost::asio::post(_ioc, [this] {
                        const ApplicationService service{_ioc};
                        const long count = service.RestartAllApplications();
                        log_info << "All applications restarted, count: " << count; });
                    return SendOkResponse(request);
                }

                case Dto::Common::ApplicationCommandType::STOP_APPLICATION: {

                    Dto::Apps::StopApplicationRequest serviceRequest = Dto::Apps::StopApplicationRequest::FromJson(clientCommand);
                    boost::asio::post(_ioc, [this, serviceRequest] {
                        const ApplicationService service{_ioc};
                        service.StopApplication(serviceRequest);
                        log_info << "Applications stopped, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name; });
                    return SendOkResponse(request);
                }

                case Dto::Common::ApplicationCommandType::STOP_ALL_APPLICATIONS: {

                    boost::asio::post(_ioc, [this] {
                        const ApplicationService service{_ioc};
                        const long count = service.StopAllApplications();
                        log_info << "All applications stopped, count: " << count; });
                    return SendOkResponse(request);
                }

                case Dto::Common::ApplicationCommandType::DELETE_APPLICATION: {

                    Dto::Apps::DeleteApplicationRequest serviceRequest = Dto::Apps::DeleteApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.DeleteApplication(serviceRequest);
                    log_info << "Application deleted, name: " << serviceRequest.name;
                    return SendOkResponse(request, serviceResponse.ToJson());
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
