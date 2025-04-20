//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/ScanResponse.h>

namespace AwsMock::Dto::DynamoDb {

    void ScanResponse::PrepareResponse(const Database::Entity::DynamoDb::Table &table) {
        FromJson(body);
        for (auto &item: items) {
            item.region = table.region;
            item.tableName = table.name;
            for (const auto &key: table.keySchemas | std::views::keys) {
                if (item.attributes.contains(key)) {
                    item.keys[key] = item.attributes.at(key);
                }
            }
        }
    }

    void ScanResponse::FromJson(const std::string &jsonString) {

        body = jsonString;

        try {

            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            tableName = Core::Bson::BsonUtils::GetStringValue(rootDocument, "TableName");
            count = Core::Bson::BsonUtils::GetLongValue(rootDocument, "Count");
            scannedCount = Core::Bson::BsonUtils::GetLongValue(rootDocument, "ScannedCount");

            if (rootDocument.find("Items") != rootDocument.end()) {
                for (const view jsonItems = rootDocument["Items"].get_array().value; const auto jsonItem: jsonItems) {
                    Database::Entity::DynamoDb::Item item;
                    item.FromDynamodb(jsonItem.get_document().value);
                    items.emplace_back(item);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ScanResponse::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "TableName", tableName);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
