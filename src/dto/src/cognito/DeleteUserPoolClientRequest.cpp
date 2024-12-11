//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/DeleteUserPoolClientRequest.h>

namespace AwsMock::Dto::Cognito {

    void DeleteUserPoolClientRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");
            clientId = Core::Bson::BsonUtils::GetStringValue(document, "ClientId");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DeleteUserPoolClientRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "ClientId", clientId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DeleteUserPoolClientRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const DeleteUserPoolClientRequest &r) {
        os << "DeleteUserPoolClientRequest=" << r.ToJson();
        return os;
    }
}// namespace AwsMock::Dto::Cognito