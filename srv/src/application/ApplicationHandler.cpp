
#include <awsmock/service/apps/ApplicationHandler.h>

namespace Awsmock::Service {

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
                    Core::Scheduler::instance().AddOneTimeTask("upload-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.UploadApplicationCode(serviceRequest);
                        log_info << "Applications uploaded, region: " << serviceRequest.region;
                    });
                    log_info << "Upload application code succeeded, name: " << serviceRequest.applicationName << ", version: " << serviceRequest.version;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::LIST_APPLICATIONS: {

                    Dto::Apps::ListApplicationCountersRequest serviceRequest = Dto::Apps::ListApplicationCountersRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.ListApplications(serviceRequest);
                    log_info << "Applications counters list, count: " << serviceResponse.applications.size();
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::LIST_APPLICATION_NAMES: {

                    Dto::Apps::ListApplicationNamesRequest serviceRequest = Dto::Apps::ListApplicationNamesRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationNamesResponse serviceResponse = _applicationService.ListApplicationNames(serviceRequest);
                    log_info << "Application names listed, region: " << serviceRequest.region;
                    // TODO: fix serialization
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApplicationCommandType::REBUILD_APPLICATION: {

                    Dto::Apps::RebuildApplicationCodeRequest serviceRequest = Dto::Apps::RebuildApplicationCodeRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("restart-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.RebuildApplication(serviceRequest);
                        log_info << "Applications rebuild, region: " << serviceRequest.region;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::ENABLE_APPLICATION: {

                    Dto::Apps::EnableApplicationRequest serviceRequest = Dto::Apps::EnableApplicationRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("enable-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.EnableApplication(serviceRequest);
                        log_info << "Application enabled, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::ENABLE_ALL_APPLICATIONS: {

                    Dto::Apps::EnableAllApplicationsRequest serviceRequest = Dto::Apps::EnableAllApplicationsRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("enable-all-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.EnableAllApplications(serviceRequest);
                        log_info << "Application enabled, region: " << serviceRequest.region;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::DISABLE_APPLICATION: {

                    Dto::Apps::DisableApplicationRequest serviceRequest = Dto::Apps::DisableApplicationRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("disable-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.DisableApplication(serviceRequest);
                        log_info << "Application disabled, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::DISABLE_ALL_APPLICATIONS: {

                    Dto::Apps::DisableAllApplicationsRequest serviceRequest = Dto::Apps::DisableAllApplicationsRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("disable-all-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.DisableAllApplications(serviceRequest);
                        log_info << "Application disabled, region: " << serviceRequest.region;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::START_APPLICATION: {

                    Dto::Apps::StartApplicationRequest serviceRequest = Dto::Apps::StartApplicationRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("start-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.StartApplication(serviceRequest);
                        log_info << "Application started, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::START_ALL_APPLICATIONS: {

                    Core::Scheduler::instance().AddOneTimeTask("start-all-application", [_logger = _logger]() mutable {
                        const long count = ApplicationService{}.StartAllApplications();
                        log_info << "All applications started, count: " << count;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::RESTART_APPLICATION: {

                    Dto::Apps::RestartApplicationRequest serviceRequest = Dto::Apps::RestartApplicationRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("restart-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.RestartApplication(serviceRequest);
                        log_info << "Applications restarted, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::RESTART_ALL_APPLICATIONS: {

                    Core::Scheduler::instance().AddOneTimeTask("restart-all-application", [_logger = _logger]() mutable {
                        const long count = ApplicationService{}.RestartAllApplications();
                        log_info << "All applications restarted, count: " << count;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::STOP_APPLICATION: {

                    Dto::Apps::StopApplicationRequest serviceRequest = Dto::Apps::StopApplicationRequest::FromJson(clientCommand);
                    Core::Scheduler::instance().AddOneTimeTask("stop-application", [serviceRequest, _logger = _logger]() mutable {
                        ApplicationService{}.StopApplication(serviceRequest);
                        log_info << "Applications stopped, region: " << serviceRequest.region << ", name: " << serviceRequest.application.name;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::STOP_ALL_APPLICATIONS: {

                    Core::Scheduler::instance().AddOneTimeTask("stop-all-application", [_logger = _logger]() mutable {
                        const long count = ApplicationService{}.StopAllApplications();
                        log_info << "All applications stopped, count: " << count;
                    });
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApplicationCommandType::DELETE_APPLICATION: {

                    Dto::Apps::DeleteApplicationRequest serviceRequest = Dto::Apps::DeleteApplicationRequest::FromJson(clientCommand);
                    Dto::Apps::ListApplicationCountersResponse serviceResponse = _applicationService.DeleteApplication(serviceRequest);
                    log_info << "Application deleted, name: " << serviceRequest.name;
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
        } catch (Core::CoreException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

}// namespace Awsmock::Service
