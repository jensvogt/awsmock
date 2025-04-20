//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/UpdateUserPoolClientRequest.h>

namespace AwsMock::Dto::Cognito {

    void UpdateUserPoolClientRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");
            clientId = Core::Bson::BsonUtils::GetStringValue(document, "ClientId");
            clientName = Core::Bson::BsonUtils::GetStringValue(document, "ClientName");
            accessTokenValidity = Core::Bson::BsonUtils::GetIntValue(document, "AccessTokenValidity");
            idTokenValidity = Core::Bson::BsonUtils::GetIntValue(document, "IdTokenValidity");
            refreshTokenValidity = Core::Bson::BsonUtils::GetIntValue(document, "RefreshTokenValidity");

            if (document.view().find("TokenValidityUnits") != document.view().end()) {
                tokenValidityUnits.FromDocument(document.view()["TokenValidityUnits"].get_document().value);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateUserPoolClientRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);
            Core::Bson::BsonUtils::SetStringValue(document, "ClientId", clientId);
            Core::Bson::BsonUtils::SetStringValue(document, "ClientName", clientName);
            Core::Bson::BsonUtils::SetIntValue(document, "AccessTokenValidity", accessTokenValidity);
            Core::Bson::BsonUtils::SetIntValue(document, "IdTokenValidity", idTokenValidity);
            Core::Bson::BsonUtils::SetIntValue(document, "TokenValidityUnits", refreshTokenValidity);

            document.append(kvp("TokenValidityUnits", tokenValidityUnits.ToDocument()));

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito