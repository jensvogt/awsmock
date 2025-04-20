//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/SignUpRequest.h>

namespace AwsMock::Dto::Cognito {

    void SignUpRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            clientId = Core::Bson::BsonUtils::GetStringValue(document, "ClientId");
            userName = Core::Bson::BsonUtils::GetStringValue(document, "Username");
            password = Core::Bson::BsonUtils::GetStringValue(document, "Password");
            secretHash = Core::Bson::BsonUtils::GetStringValue(document, "SecretHash");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SignUpRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ClientId", clientId);
            Core::Bson::BsonUtils::SetStringValue(document, "Username", userName);
            Core::Bson::BsonUtils::SetStringValue(document, "Password", password);
            Core::Bson::BsonUtils::SetStringValue(document, "SecretHash", secretHash);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito