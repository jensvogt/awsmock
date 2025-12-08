
#include "awsmock/core/exception/BadRequestException.h"
#include "awsmock/core/exception/NotFoundException.h"
#include "awsmock/dto/container/ListContainerRequest.h"
#include "awsmock/dto/container/ListStatsRequest.h"


#include <awsmock/service/container/ContainerHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> ContainerHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Container POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ContainerClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {
                case Dto::Common::ContainerCommandType::LIST_CONTAINERS: {
                    Dto::Docker::ListContainerRequest containerRequest = Dto::Docker::ListContainerRequest::FromJson(clientCommand);
                    Dto::Docker::ListContainerResponse serviceResponse = _containerService.ListContainers();
                    log_info << "List containers, count: " << serviceResponse.containerList.size();
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ContainerCommandType::LIST_CONTAINER_STATS: {
                    Dto::Docker::ListStatsRequest containerRequest = Dto::Docker::ListStatsRequest::FromJson(clientCommand);
                    Dto::Docker::ListStatsResponse serviceResponse = _containerService.ListContainerStats(containerRequest);
                    log_info << "List container stats, count: " << serviceResponse.statistics.size();
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
} // namespace AwsMock::Service
