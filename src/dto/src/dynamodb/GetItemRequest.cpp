//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/GetItemRequest.h>

namespace AwsMock::Dto::DynamoDb {

    void GetItemRequest::FromJson(const std::string &jsonString) {

        // Save original body
        body = jsonString;

        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            tableName = Core::Bson::BsonUtils::GetStringValue(rootDocument, "TableName");

            if (rootDocument.view().find("Keys") != rootDocument.view().end()) {
                const view keyDocument = rootDocument.view()["Keys"].get_document().value;
                GetItemKey getItemKey;
                for (const auto &element: keyDocument) {
                    std::string key = bsoncxx::string::to_string(element.key());
                    std::string value = bsoncxx::string::to_string(element[key].get_string().value);
                    getItemKey.type = key;
                    getItemKey.value = Core::Bson::BsonUtils::GetStringValue(element);
                }
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetItemRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "TableName", tableName);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
