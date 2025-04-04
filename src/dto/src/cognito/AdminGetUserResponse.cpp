//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/AdminGetUserResponse.h>

namespace AwsMock::Dto::Cognito {

    void AdminGetUserResponse::FromJson(const std::string &payload) {

        try {

            const value document = bsoncxx::from_json(payload);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            userName = Core::Bson::BsonUtils::GetStringValue(document, "Username");

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

    std::string AdminGetUserResponse::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Id", id);
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "Username", userName);
            Core::Bson::BsonUtils::SetStringValue(document, "Password", password);
            Core::Bson::BsonUtils::SetBoolValue(document, "Enabled", enabled);
            Core::Bson::BsonUtils::SetStringValue(document, "UserStatus", UserStatusToString(userStatus));
            Core::Bson::BsonUtils::SetDateValue(document, "Created", created);
            Core::Bson::BsonUtils::SetDateValue(document, "Modified", modified);

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string AdminGetUserResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const AdminGetUserResponse &r) {
        os << "AdminGetUserResponse=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Cognito