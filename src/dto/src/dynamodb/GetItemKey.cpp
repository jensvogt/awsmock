//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/GetItemKey.h>

namespace AwsMock::Dto::DynamoDb {

    void GetItemKey::FromJson(const std::string &jsonString) {

        // Save original body
        std::string body = jsonString;

        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            keyType = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Type");
            keyValue = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Value");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetItemKey::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "Type", keyType);
            Core::Bson::BsonUtils::SetStringValue(document, "Value", keyValue);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
