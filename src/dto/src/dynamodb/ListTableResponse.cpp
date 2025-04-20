//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/ListTableResponse.h>

namespace AwsMock::Dto::DynamoDb {

    void ListTableResponse::ScanResponse() {
        FromJson(body, headers);
    }

    void ListTableResponse::FromJson(const std::string &body, const std::map<std::string, std::string> &headers) {

        if (body.empty()) {
            log_info << "Empty response from DynamoDB";
            return;
        }

        this->body = body;
        this->headers = headers;

        try {

            if (const value documentValue = bsoncxx::from_json(body); documentValue.find("TableNames") != documentValue.end()) {
                Core::Bson::FromBsonStringArray(documentValue, "TableNames", &tableNames);
            }

        } catch (std::exception &exc) {
            log_error << exc.what() << " body: " << body;
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListTableResponse::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "LastEvaluatedTableName", lastEvaluatedTableName);

            if (!tableNames.empty()) {
                array array;
                for (const auto &tableName: tableNames) {
                    array.append(tableName);
                }
                document.append(kvp("TableNames", array));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
