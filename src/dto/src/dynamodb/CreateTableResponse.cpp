//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/dynamodb/CreateTableResponse.h>

namespace AwsMock::Dto::DynamoDb {

    void CreateTableResponse::ScanResponse() {
        FromJson(body, headers);
    }

    void CreateTableResponse::FromJson(const std::string &body, const std::map<std::string, std::string> &headers) {

        this->body = body;
        this->headers = headers;

        try {

            if (const value rootDocument = bsoncxx::from_json(body); rootDocument.view().find("Table") != rootDocument.view().end()) {

                view tableDocument = rootDocument.view()["Table"].get_document().value;
                tableName = Core::Bson::BsonUtils::GetStringValue(tableDocument, "TableName");
                tableArn = Core::Bson::BsonUtils::GetStringValue(tableDocument, "TableArn");
                itemCount = Core::Bson::BsonUtils::GetLongValue(tableDocument, "ItemCount");
                tableStatus = TableStatusTypeFromString(Core::Bson::BsonUtils::GetStringValue(tableDocument, "TableStatus"));

                // Attributes
                if (tableDocument.find("AttributeDefinitions") != tableDocument.end()) {
                    for (const bsoncxx::array::view jsonArray = tableDocument["AttributeDefinitions"].get_array().value; const auto &element: jsonArray) {
                        view jsonObject = element.get_document().value;
                        std::string name = Core::Bson::BsonUtils::GetStringValue(jsonObject, "AttributeName");
                        const std::string type = Core::Bson::BsonUtils::GetStringValue(jsonObject, "AttributeType");
                        attributes[name] = type;
                    }
                }

                // Key schemas
                if (tableDocument.find("KeySchema") != tableDocument.end()) {
                    for (const bsoncxx::array::view jsonArray = tableDocument["KeySchema"].get_array().value; const auto &element: jsonArray) {
                        view jsonObject = element.get_document().value;
                        std::string name = Core::Bson::BsonUtils::GetStringValue(jsonObject, "AttributeName");
                        const std::string type = Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyType");
                        keySchemas[name] = type;
                    }
                }

                // Tags
                if (tableDocument.find("Tags") != tableDocument.end()) {
                    for (const bsoncxx::array::view jsonArray = tableDocument["Tags"].get_array().value; const auto &element: jsonArray) {
                        view jsonObject = element.get_document().value;
                        std::string key = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Key");
                        const std::string value = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Value");
                        tags[key] = value;
                    }
                }

                // Provisioned throughput
                // TODO: Add provisioned throughput
                //provisionedThroughput.FromDocument(tableDocument["ProvisionedThroughput"].get_document().value);
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateTableResponse::ToJson() {

        try {

            document document;

            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "TableClass", tableClass);
            Core::Bson::BsonUtils::SetStringValue(document, "TableName", tableName);

            if (!tags.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: tags) {
                    bsoncxx::builder::basic::document element;
                    Core::Bson::BsonUtils::SetStringValue(element, "Key", fst);
                    Core::Bson::BsonUtils::SetStringValue(element, "Value", snd);
                    jsonArray.append(element);
                }
                document.append(kvp("Tags", jsonArray));
            }

            if (!attributes.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: attributes) {
                    bsoncxx::builder::basic::document element;
                    Core::Bson::BsonUtils::SetStringValue(element, "AttributeName", fst);
                    Core::Bson::BsonUtils::SetStringValue(element, "AttributeType", snd);
                    jsonArray.append(element);
                }
                document.append(kvp("AttributeDefinitions", jsonArray));
            }

            if (!keySchemas.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: keySchemas) {
                    bsoncxx::builder::basic::document element;
                    Core::Bson::BsonUtils::SetStringValue(element, "AttributeName", fst);
                    Core::Bson::BsonUtils::SetStringValue(element, "KeyType", snd);
                    jsonArray.append(element);
                }
                document.append(kvp("KeySchema", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
