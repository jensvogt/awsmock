//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/DeleteTableResponse.h>

namespace AwsMock::Dto::DynamoDb {

    void DeleteTableResponse::FromJson(const std::string &body, const std::map<std::string, std::string> &headerMap) {

        this->body = body;
        this->headers = headerMap;

        try {
            if (const value rootDocument = bsoncxx::from_json(body); rootDocument.view().find("TableDescription") != rootDocument.view().end()) {
                view tableDescriptionDocument = rootDocument.view()["TableDescription"].get_document().value;
                region = Core::Bson::BsonUtils::GetStringValue(tableDescriptionDocument, "Region");
                tableName = Core::Bson::BsonUtils::GetStringValue(tableDescriptionDocument, "TableName");
                tableArn = Core::Bson::BsonUtils::GetStringValue(tableDescriptionDocument, "TableArn");
                tableStatus = TableStatusTypeFromString(Core::Bson::BsonUtils::GetStringValue(tableDescriptionDocument, "TableStatus"));
                size = Core::Bson::BsonUtils::GetLongValue(tableDescriptionDocument, "TableSizeBytes");
                itemCount = Core::Bson::BsonUtils::GetLongValue(tableDescriptionDocument, "ItemCount");

                // Provisioned throughput
                if (tableDescriptionDocument.find("ProvisionedThroughput") != tableDescriptionDocument.end()) {
                    provisionedThroughput.FromDocument(tableDescriptionDocument["ProvisionedThroughput"].get_document().value);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DeleteTableResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "TableName", tableName);
            Core::Bson::BsonUtils::SetStringValue(document, "TableArn", tableArn);

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
