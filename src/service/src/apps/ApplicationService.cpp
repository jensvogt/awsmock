
//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/apps/ApplicationService.h>

namespace AwsMock::Service {

    ApplicationService::ApplicationService() : _database(Database::ApplicationDatabase::instance()) {
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::CreateApplication(const Dto::Apps::CreateApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "create_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "create_application");
        log_debug << "Create application request, region:  " << request.region << " name: " << request.application.name;

        if (_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application exists already, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application exists already, region: " + request.region + " name: " + request.application.name);
        }

        try {
            // Generate application ID
            Database::Entity::Apps::Application application = Dto::Apps::Mapper::map(request.application);

            application = _database.CreateApplication(application);

            Dto::Apps::ListApplicationCountersRequest listRequest{};
            listRequest.requestId = request.requestId;
            listRequest.region = request.region;
            listRequest.user = request.user;
            listRequest.prefix = request.prefix;
            listRequest.pageSize = request.pageSize;
            listRequest.pageIndex = request.pageIndex;
            log_trace << "Application created, application: " + application.ToJson();
            return ListApplications(listRequest);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Apps::GetApplicationResponse ApplicationService::GetApplication(const Dto::Apps::GetApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "get_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "get_application");
        log_debug << "Get application request, region:  " << request.region << " name: " << request.name;

        if (!_database.ApplicationExists(request.region, request.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.name);
        }

        try {
            const Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.name);

            Dto::Apps::GetApplicationResponse getRequest{};
            getRequest.requestId = request.requestId;
            getRequest.region = request.region;
            getRequest.user = request.user;
            getRequest.application = Dto::Apps::Mapper::map(application);
            log_trace << "Application retrieved, application: " + application.ToJson();
            return getRequest;

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

    Dto::Apps::ListApplicationCountersResponse ApplicationService::DeleteApplication(const Dto::Apps::DeleteApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "delete_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "delete_application");
        log_debug << "Delete application request, region:  " << request.region << " name: " << request.name;

        if (!_database.ApplicationExists(request.region, request.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.name);
        }

        try {

            const long count = _database.DeleteApplication(request.region, request.name);
            log_debug << "Application deleted, count: " << count;

            Dto::Apps::ListApplicationCountersRequest listRequest{};
            listRequest.requestId = request.requestId;
            listRequest.region = request.region;
            listRequest.user = request.user;
            listRequest.prefix = request.prefix;
            listRequest.pageSize = request.pageSize;
            listRequest.pageIndex = request.pageIndex;
            log_trace << "Application deleted, name: " + request.name;
            return ListApplications(listRequest);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }
}// namespace AwsMock::Service
