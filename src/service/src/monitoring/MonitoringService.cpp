//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/monitoring/MonitoringService.h>

namespace AwsMock::Service {

    Dto::Monitoring::GetCountersResponse MonitoringService::GetCounters(const Dto::Monitoring::GetCountersRequest &request) const {
        log_trace << "Get counters request, request: " << request.ToString();

        try {
            Dto::Monitoring::GetCountersResponse response;

            // Get counters from database
            response.counters = _database.GetMonitoringValues(request.name, request.start, request.end, request.step);

            log_trace << "Monitoring get counter, count: " << response.counters.size();

            return response;

        } catch (std::exception &exc) {
            log_error << "Monitoring get counters failed, message: " << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::Monitoring::GetMultiCountersResponse MonitoringService::GetMultiCounters(const Dto::Monitoring::GetCountersRequest &request) const {
        log_trace << "Get multiple counters request, request: " << request.ToString();

        try {
            Dto::Monitoring::GetMultiCountersResponse response;

            // Get counters from database
            for (std::vector<std::string> series = _database.GetDistinctLabelValues(request.labelName); const auto &labelValue: series) {
                response.counters[labelValue] = _database.GetMonitoringValues(request.name, request.start, request.end, request.step, request.labelName, labelValue);
            }

            log_trace << "Monitoring get counter, count: " << response.counters.size();

            return response;

        } catch (std::exception &exc) {
            log_error << "Monitoring get multiple counters failed, message: " << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

}// namespace AwsMock::Service
