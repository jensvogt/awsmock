
#include <awsmock/service/monitoring/MonitoringHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> MonitoringHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "Monitoring POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::MonitoringClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::MonitoringCommandType::GET_COUNTERS: {

                    Dto::Monitoring::GetCountersRequest monitoringRequest = Dto::Monitoring::GetCountersRequest::FromJson(clientCommand.payload);
                    Dto::Monitoring::GetCountersResponse response = _monitoringService.GetCounters(monitoringRequest);

                    log_debug << "Get counters, name: " << monitoringRequest.name << " count: " << response.counters.size();
                    return SendOkResponse(request, response.ToJson());
                }
                case Dto::Common::MonitoringCommandType::UNKNOWN:
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
        return SendBadRequestError(request, "Unknown method");
    }

}// namespace AwsMock::Service
