//
// Created by vogje01 on 9/2/25.
//

#include <awsmock/service/apigateway/ApiGatewayService.h>

namespace AwsMock::Service {

    Dto::ApiGateway::CreateApiKeyResponse ApiGatewayService::CreateApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_api_key");
        log_debug << "Create API key request, region:  " << request.region << " customerId: " << request.customerId;

        if (_apiGatewayDatabase.ApiKeyExists(request.region, request.name)) {
            log_error << "API key exists already, region: " << request.region << " name: " << request.customerId;
            throw Core::ServiceException("API key exists already, region: " + request.region + " name: " + request.customerId);
        }

        try {

            // Generate API key
            Database::Entity::ApiGateway::Key key = Dto::ApiGateway::Mapper::map(request);
            key = _apiGatewayDatabase.CreateKey(key);
            log_trace << "Api key created, name: " + key.ToJson();
            return Dto::ApiGateway::Mapper::map(request, key);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Service