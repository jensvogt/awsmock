//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/AdminCreateUserRequest.h>

namespace AwsMock::Dto::Cognito {

    void AdminCreateUserRequest::FromJson(const std::string &payload) {

        try {

            const value document = bsoncxx::from_json(payload);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");
            userName = Core::Bson::BsonUtils::GetStringValue(document, "Username");
            temporaryPassword = Core::Bson::BsonUtils::GetStringValue(document, "TemporaryPassword");
            messageAction = MessageActionFromString(Core::Bson::BsonUtils::GetStringValue(document, "MessageAction"));

            // Message action
            if (document.find("UserAttributes") != document.end()) {
                array array;
                for (const auto &element: document["UserAttributes"].get_array().value) {
                    UserAttribute userAttribute;
                    userAttribute.FromDocument(element.get_document());
                    userAttributes.emplace_back(userAttribute);
                }
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string AdminCreateUserRequest::ToJson() {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "Username", userName);
            Core::Bson::BsonUtils::SetStringValue(document, "TemporaryPassword", temporaryPassword);

            return Core::Bson ::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito