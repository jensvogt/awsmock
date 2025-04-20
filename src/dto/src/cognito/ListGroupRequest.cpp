//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/ListGroupsRequest.h>

namespace AwsMock::Dto::Cognito {

    void ListGroupsRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");
            nextToken = Core::Bson::BsonUtils::GetStringValue(document, "NextToken");
            limit = Core::Bson::BsonUtils::GetIntValue(document, "Limit");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListGroupsRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "NextToken", nextToken);
            Core::Bson::BsonUtils::SetIntValue(document, "Limit", limit);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito