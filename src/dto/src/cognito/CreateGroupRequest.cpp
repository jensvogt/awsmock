//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/CreateGroupRequest.h>

namespace AwsMock::Dto::Cognito {

    void CreateGroupRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            groupName = Core::Bson::BsonUtils::GetStringValue(document, "GroupName");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");
            precedence = Core::Bson::BsonUtils::GetIntValue(document, "Precedence");
            description = Core::Bson::BsonUtils::GetStringValue(document, "Description");
            roleArn = Core::Bson::BsonUtils::GetStringValue(document, "RoleArn");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateGroupRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "GroupName", groupName);
            Core::Bson::BsonUtils::SetStringValue(document, "Description", description);
            Core::Bson::BsonUtils::SetIntValue(document, "Precedence", precedence);
            Core::Bson::BsonUtils::SetStringValue(document, "RoleArn", roleArn);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito