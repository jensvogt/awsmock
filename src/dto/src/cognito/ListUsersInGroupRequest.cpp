//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/ListUsersInGroupRequest.h>

namespace AwsMock::Dto::Cognito {

    void ListUsersInGroupRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");
            groupName = Core::Bson::BsonUtils::GetStringValue(document, "GroupName");
            limit = Core::Bson::BsonUtils::GetIntValue(document, "Limit");
            nextToken = Core::Bson::BsonUtils::GetStringValue(document, "NextToken");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListUsersInGroupRequest::ToJson() const {
        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "GroupName", groupName);
            Core::Bson::BsonUtils::SetIntValue(document, "Limit", limit);
            Core::Bson::BsonUtils::SetStringValue(document, "NextToken", nextToken);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito