//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/GetItemResponse.h>

namespace AwsMock::Dto::DynamoDb {

    void GetItemResponse::FromJson(const std::string &jsonString) {

        body = jsonString;

        try {

            if (const value document = bsoncxx::from_json(jsonString); document.view().find("Item") != document.view().end()) {
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetItemResponse::ToJson() {

        /* Todo
        try {
            Poco::JSON::Object rootJson;
            rootJson.set("Region", region);
            rootJson.set("TableName", tableName);

            return Core::JsonUtils::ToJsonString(rootJson);

        } catch (Poco::Exception &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }*/
        return {};
    }

}// namespace AwsMock::Dto::DynamoDb
