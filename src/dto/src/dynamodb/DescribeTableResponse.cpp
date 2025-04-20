//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/DescribeTableResponse.h>

namespace AwsMock::Dto::DynamoDb {

    void DescribeTableResponse::PrepareResponse() {
        FromJson(body, headers);
    }

    void DescribeTableResponse::FromJson(const std::string &body, const std::map<std::string, std::string> &headers) {

        this->body = body;
        this->headers = headers;

        try {

            const value rootDocument = bsoncxx::from_json(body);
            view tableDocument = rootDocument.view()["Table"].get_document().value;
            tableName = Core::Bson::BsonUtils::GetStringValue(tableDocument, "TableName");
            tableSize = Core::Bson::BsonUtils::GetLongValue(tableDocument, "TableSizeBytes");
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
            provisionedThroughput.FromDocument(tableDocument["ProvisionedThroughput"].get_document().value);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DescribeTableResponse::ToJson() {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "TableName", tableName);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "TableSizeBytes", tableSize);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "ItemCount", itemCount);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "TableArn", tableArn);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "TableStatus", TableStatusTypeToString(tableStatus));
            rootDocument.append(kvp("ProvisionedThroughput", provisionedThroughput.ToDocument()));

            if (!keySchemas.empty()) {
                array jsonKeySchemasArray;
                for (const auto &[fst, snd]: keySchemas) {
                    document object;
                    object.append(kvp("AttributeName", fst));
                    object.append(kvp("KeyType", snd));
                    jsonKeySchemasArray.append(object);
                }
                rootDocument.append(kvp("KeySchema", jsonKeySchemasArray));
            }

            if (!attributes.empty()) {
                array jsonAttributesArray;
                for (const auto &[fst, snd]: attributes) {
                    document object;
                    object.append(kvp("AttributeName", fst));
                    object.append(kvp("AttributeType", snd));
                    jsonAttributesArray.append(object);
                }
                rootDocument.append(kvp("AttributeDefinitions", jsonAttributesArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
