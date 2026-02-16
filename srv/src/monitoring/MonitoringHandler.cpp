
#include <awsmock/service/monitoring/MonitoringHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> MonitoringHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Monitoring POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::MonitoringClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        // Cloudwatch request
        if (Core::StringUtils::Contains(clientCommand.payload, "Action")) {
            return SendResponse(request, http::status::ok, {});
        }

        try {
            switch (clientCommand.command) {
                case Dto::Common::MonitoringCommandType::GET_COUNTERS: {
                    const Dto::Monitoring::GetCountersRequest monitoringRequest = Dto::Monitoring::GetCountersRequest::FromJson(clientCommand);
                    const Dto::Monitoring::GetCountersResponse response = _monitoringService.GetCounters(monitoringRequest);
                    log_debug << "Get counters, name: " << monitoringRequest.name << " count: " << response.counters.size();
                    return SendResponse(request, http::status::ok, response.ToJson());
                }

                case Dto::Common::MonitoringCommandType::GET_MULTI_COUNTERS: {
                    const Dto::Monitoring::GetCountersRequest monitoringRequest = Dto::Monitoring::GetCountersRequest::FromJson(clientCommand);
                    const Dto::Monitoring::GetMultiCountersResponse response = _monitoringService.GetMultiCounters(monitoringRequest);
                    log_debug << "Get multiple counters, name: " << monitoringRequest.name << " count: " << response.counters.size();
                    return SendResponse(request, http::status::ok, response.ToJson());
                }

                default:
                    log_error << "Unknown method";
                    return SendResponse(request, http::status::bad_request, "Unknown method");
            }
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }
}// namespace AwsMock::Service
