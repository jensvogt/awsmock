//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/cognito/UserPoolClient.h>

#include "awsmock/dto/cognito/model/AuthFlow.h"

namespace AwsMock::Database::Entity::Cognito {

    view_or_value<view, value> UserPoolClient::ToDocument() const {

        document userPoolDocument;
        userPoolDocument.append(kvp("userPoolId", userPoolId));
        userPoolDocument.append(kvp("clientId", clientId));
        userPoolDocument.append(kvp("clientName", clientName));
        userPoolDocument.append(kvp("clientSecret", clientSecret));
        userPoolDocument.append(kvp("generateSecret", generateSecret));
        userPoolDocument.append(kvp("accessTokenValidity", bsoncxx::types::b_int64(accessTokenValidity)));
        userPoolDocument.append(kvp("idTokenValidity", bsoncxx::types::b_int64(idTokenValidity)));
        userPoolDocument.append(kvp("refreshTokenValidity", bsoncxx::types::b_int64(refreshTokenValidity)));
        userPoolDocument.append(kvp("created", bsoncxx::types::b_date(created)));
        userPoolDocument.append(kvp("modified", bsoncxx::types::b_date(modified)));

        if (!explicitAuthFlows.empty()) {
            array authFlowsArray;
            for (const auto &f: explicitAuthFlows) {
                authFlowsArray.append(f);
            }
            userPoolDocument.append(kvp("authFlows", authFlowsArray));
        }

        return userPoolDocument.extract();
    }

    void UserPoolClient::FromDocument(const std::optional<view> &mResult) {

        try {

            userPoolId = Core::Bson::BsonUtils::GetStringValue(mResult, "userPoolId");
            clientId = Core::Bson::BsonUtils::GetStringValue(mResult, "clientId");
            clientName = Core::Bson::BsonUtils::GetStringValue(mResult, "clientName");
            clientSecret = Core::Bson::BsonUtils::GetStringValue(mResult, "clientSecret");
            accessTokenValidity = Core::Bson::BsonUtils::GetLongValue(mResult, "accessTokenValidity");
            idTokenValidity = Core::Bson::BsonUtils::GetLongValue(mResult, "idTokenValidity");
            refreshTokenValidity = Core::Bson::BsonUtils::GetLongValue(mResult, "refreshTokenValidity");
            generateSecret = Core::Bson::BsonUtils::GetBoolValue(mResult, "generateSecret");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            if (mResult.value()["explicitAuthFlows"].length() > 0) {
                Core::Bson::FromBsonArray(mResult.value(), "explicitAuthFlows", &explicitAuthFlows);
            }

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::Cognito