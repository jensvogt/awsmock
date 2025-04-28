//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/DeleteUserPoolRequest.h>

namespace AwsMock::Dto::Cognito {

    void DeleteUserPoolRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DeleteUserPoolRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito