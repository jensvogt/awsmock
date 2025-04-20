//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/CreateEventSourceMappingsRequest.h>

namespace AwsMock::Dto::Lambda {

    void CreateEventSourceMappingsRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionName = Core::Bson::BsonUtils::GetStringValue(document, "FunctionName");
            eventSourceArn = Core::Bson::BsonUtils::GetStringValue(document, "EventSourceArn");
            batchSize = Core::Bson::BsonUtils::GetIntValue(document, "BatchSize");
            maximumBatchingWindowInSeconds = Core::Bson::BsonUtils::GetIntValue(document, "MaximumBatchingWindowInSeconds");
            enabled = Core::Bson::BsonUtils::GetBoolValue(document, "Enabled");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateEventSourceMappingsRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "User", user);
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionName", functionName);
            Core::Bson::BsonUtils::SetStringValue(document, "EventSourceArn", eventSourceArn);
            Core::Bson::BsonUtils::SetIntValue(document, "BatchSize", batchSize);
            Core::Bson::BsonUtils::SetIntValue(document, "MaximumBatchingWindowInSeconds", maximumBatchingWindowInSeconds);
            Core::Bson::BsonUtils::SetBoolValue(document, "Enabled", enabled);
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Lambda
