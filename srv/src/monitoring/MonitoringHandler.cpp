
#include <awsmock/service/monitoring/MonitoringHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> MonitoringHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Monitoring POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::MonitoringClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        // Cloudwatch request
        if (Core::StringUtils::Contains(clientCommand.payload, "Action")) {
            return SendOkResponse(request, {});
        }

        try {
            switch (clientCommand.command) {
                case Dto::Common::MonitoringCommandType::GET_COUNTERS: {
                    const Dto::Monitoring::GetCountersRequest monitoringRequest = Dto::Monitoring::GetCountersRequest::FromJson(clientCommand);
                    const Dto::Monitoring::GetCountersResponse response = _monitoringService.GetCounters(monitoringRequest);
                    log_debug << "Get counters, name: " << monitoringRequest.name << " count: " << response.counters.size();
                    return SendOkResponse(request, response.ToJson());
                }

                case Dto::Common::MonitoringCommandType::GET_MULTI_COUNTERS: {
                    const Dto::Monitoring::GetCountersRequest monitoringRequest = Dto::Monitoring::GetCountersRequest::FromJson(clientCommand);
                    const Dto::Monitoring::GetMultiCountersResponse response = _monitoringService.GetMultiCounters(monitoringRequest);
                    log_debug << "Get multiple counters, name: " << monitoringRequest.name << " count: " << response.counters.size();
                    return SendOkResponse(request, response.ToJson());
                }

                default:
                    log_error << "Unknown method";
                    return SendBadRequestError(request, "Unknown method");
            }
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        }
    }
}// namespace AwsMock::Service
