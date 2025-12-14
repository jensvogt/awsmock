
#include <awsmock/service/container/ContainerHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> ContainerHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Container POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ContainerClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::ContainerCommandType::START_CONTAINER: {
                    Dto::Docker::StartContainerRequest containerRequest = Dto::Docker::StartContainerRequest::FromJson(clientCommand);
                    _containerService.StartContainer(containerRequest.containerId);
                    log_info << "Start container, containerId: " << containerRequest.containerId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ContainerCommandType::STOP_CONTAINER: {
                    Dto::Docker::StopContainerRequest containerRequest = Dto::Docker::StopContainerRequest::FromJson(clientCommand);
                    _containerService.StopContainer(containerRequest.containerId);
                    log_info << "Stop container, containerId: " << containerRequest.containerId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ContainerCommandType::KILL_CONTAINER: {
                    Dto::Docker::KillContainerRequest containerRequest = Dto::Docker::KillContainerRequest::FromJson(clientCommand);
                    _containerService.KillContainer(containerRequest.containerId);
                    log_info << "Kill container, containerId: " << containerRequest.containerId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ContainerCommandType::RESTART_CONTAINER: {
                    Dto::Docker::KillContainerRequest containerRequest = Dto::Docker::KillContainerRequest::FromJson(clientCommand);
                    _containerService.KillContainer(containerRequest.containerId);
                    _containerService.StartContainer(containerRequest.containerId);
                    log_info << "Restart container, containerId: " << containerRequest.containerId;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ContainerCommandType::LIST_CONTAINERS: {
                    Dto::Docker::ListContainerRequest containerRequest = Dto::Docker::ListContainerRequest::FromJson(clientCommand);
                    Dto::Docker::ListContainerResponse serviceResponse = _containerService.ListContainers();
                    log_info << "List containers, count: " << serviceResponse.containerList.size();
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ContainerCommandType::LIST_CONTAINER_STATS: {
                    Dto::Docker::ListStatsResponse serviceResponse = _containerService.ListContainerStats();
                    log_info << "List container stats, count: " << serviceResponse.containerStats.size();
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                default:
                    return SendResponse(request, http::status::bad_request, "Unknown action");
            }
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        }
    }
}// namespace AwsMock::Service
