//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/AdminRemoveUserFromGroupRequest.h>

namespace AwsMock::Dto::Cognito {

    void AdminRemoveUserFromGroupRequest::FromJson(const std::string &payload) {

        try {

            const value document = bsoncxx::from_json(payload);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            groupName = Core::Bson::BsonUtils::GetStringValue(document, "GroupName");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");
            userName = Core::Bson::BsonUtils::GetStringValue(document, "Username");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string AdminRemoveUserFromGroupRequest::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "GroupName", groupName);
            Core::Bson::BsonUtils::SetStringValue(document, "Username", userName);

            return Core::Bson ::BsonUtils::ToJsonString(document);

        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            throw Core::JsonException(exc.message());
        }
    }

}// namespace AwsMock::Dto::Cognito