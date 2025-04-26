//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/core/AwsUtils.h>
#include <awsmock/dto/dynamodb/ListTableRequest.h>

namespace AwsMock::Dto::DynamoDb {

    void ListTableRequest::PrepareRequest() {
        body = ToJson();
        headers["Accept-Encoding"] = "identity";
        headers["Content-Type"] = "application/x-amz-json-1.0";
        headers["User-Agent"] = "aws-cli/2.22.16 md/awscrt#0.23.4 ua/2.0 os/linux#6.8.0-49-generic md/arch#x86_64 lang/python#3.12.6 md/pyimpl#CPython cfg/retry-mode#standard md/installer#exe md/distrib#ubuntu.24 md/prompt#off md/command#dynamodb.list-tables";
        headers["X-Amz-Target"] = "DynamoDB_20120810.ListTables";
        headers["X-Amz-Date"] = Core::DateTimeUtils::HttpFormatNow();
        headers["X-Amz-Security-Token"] = "none";
        headers["Content-Length"] = std::to_string(body.size());
    }

    void ListTableRequest::FromJson(const std::string &jsonString) {

        // Save original body
        body = jsonString;

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            exclusiveStartTableName = Core::Bson::BsonUtils::GetStringValue(document, "ExclusiveStartTableName");
            limit = Core::Bson::BsonUtils::GetIntValue(document, "Limit");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListTableRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ExclusiveStartTableName", exclusiveStartTableName);
            Core::Bson::BsonUtils::SetIntValue(document, "Limit", limit);

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
