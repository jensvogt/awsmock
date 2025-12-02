//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/ListEventSourceMappingsRequest.h>

namespace AwsMock::Dto::Lambda {

    void ListEventSourceMappingsRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionName = Core::Bson::BsonUtils::GetStringValue(document, "FunctionName");
            eventSourceArn = Core::Bson::BsonUtils::GetStringValue(document, "EventSourceArn");
            marker = Core::Bson::BsonUtils::GetStringValue(document, "Marker");
            maxItems = Core::Bson::BsonUtils::GetIntValue(document, "MaxItems");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListEventSourceMappingsRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "User", user);
            Core::Bson::BsonUtils::SetStringValue(document, "FunctionName", functionName);
            Core::Bson::BsonUtils::SetStringValue(document, "EventSourceArn", eventSourceArn);
            Core::Bson::BsonUtils::SetStringValue(document, "Marker", marker);
            Core::Bson::BsonUtils::SetIntValue(document, "MaxItems", maxItems);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Lambda
