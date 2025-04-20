//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/SignUpResponse.h>

namespace AwsMock::Dto::Cognito {

    void SignUpResponse::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "NextToken");
            userConfirmed = Core::Bson::BsonUtils::GetBoolValue(document, "UserConfirmed");
            userSub = Core::Bson::BsonUtils::GetStringValue(document, "UserSub");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SignUpResponse::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetBoolValue(document, "UserConfirmed", userConfirmed);
            Core::Bson::BsonUtils::SetStringValue(document, "UserSub", userSub);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito