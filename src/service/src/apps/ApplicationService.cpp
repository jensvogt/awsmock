
//
// Created by vogje01 on 30/05/2023.
//

#include "awsmock/dto/apps/mapper/Mapper.h"


#include <awsmock/service/apps/ApplicationService.h>

namespace AwsMock::Service {

    ApplicationService::ApplicationService() : _database(Database::ApplicationDatabase::instance()) {
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::Apps::CreateApplicationResponse ApplicationService::CreateApplication(const Dto::Apps::CreateApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "create_user_pool");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "create_user_pool");
        log_debug << "Create application request, region:  " << request.region << " name: " << request.application.name;

        if (_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application exists already, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application exists already, region: " + request.region + " name: " + request.application.name);
        }

        try {
            // Generate application ID
            Database::Entity::Apps::Application application = Dto::Apps::Mapper::map(request.application);

            application = _database.CreateApplication(application);

            Dto::Apps::CreateApplicationResponse response{};
            response.requestId = request.requestId;
            response.region = request.region;
            response.user = request.user;
            response.application = Dto::Apps::Mapper::map(application);
            log_trace << "Application created, response: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::ListApplications(const Dto::Apps::ListApplicationCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "list_applications");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "list_applications");
        log_debug << "List applications request";

        try {

            const std::vector<Database::Entity::Apps::Application> applications = _database.ListApplications(request.region);
            log_trace << "Got applications, region: " << request.region;

            // Prepare response
            Dto::Apps::ListApplicationCountersResponse response;
            response.region = request.region;
            response.user = request.user;
            response.requestId = request.requestId;
            response.applications = Dto::Apps::Mapper::map(applications);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Service
