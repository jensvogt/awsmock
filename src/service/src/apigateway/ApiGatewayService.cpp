//
// Created by vogje01 on 9/2/25.
//

#include <awsmock/service/apigateway/ApiGatewayService.h>

namespace AwsMock::Service {

    Dto::ApiGateway::CreateApiKeyResponse ApiGatewayService::CreateApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_api_key");
        log_debug << "Create API key request, region:  " << request.region << " customerId: " << request.customerId;

        if (_apiGatewayDatabase.ApiKeyExists(request.region, request.customerId)) {
            log_error << "API key exists already, region: " << request.region << " name: " << request.customerId;
            throw Core::ServiceException("API key exists already, region: " + request.region + " name: " + request.customerId);
        }

        try {

            // Generate API key
            Database::Entity::ApiGateway::Key key = Dto::ApiGAteway::Mapper::map(request);
            /*application.region = request.region;
            application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
            application = _database.CreateApplication(application);

            // Save the base64 encoded file
            const std::string fullBase64File = WriteBase64File(request.code, application, request.application.version);

            // Create the application asynchronously
            const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
            ApplicationCreator applicationCreator;
            boost::asio::post(_ioc, [applicationCreator, fullBase64File, application, instanceId] {
                applicationCreator(fullBase64File, application.region, application.name, instanceId);
            });

            Dto::Apps::ListApplicationCountersRequest listRequest{};
            listRequest.requestId = request.requestId;
            listRequest.region = request.region;
            listRequest.user = request.user;
            listRequest.prefix = request.prefix;
            listRequest.pageSize = request.pageSize;
            listRequest.pageIndex = request.pageIndex;
            log_trace << "Application created, application: " + application.ToJson();*/
            return ListApplications(listRequest);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Service