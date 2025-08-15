//
// Created by vogje01 on 03/09/2023.
//

#include "awsmock/core/exception/DatabaseException.h"


#include <awsmock/entity/cognito/UserPoolClient.h>

namespace AwsMock::Database::Entity::Cognito {

    view_or_value<view, value> UserPoolClient::ToDocument() const {

        view_or_value<view, value> userPoolDocument = make_document(
                kvp("userPoolId", userPoolId),
                kvp("clientId", clientId),
                kvp("clientName", clientName),
                kvp("clientSecret", clientSecret),
                kvp("generateSecret", generateSecret),
                kvp("accessTokenValidity", bsoncxx::types::b_int64(accessTokenValidity)),
                kvp("idTokenValidity", bsoncxx::types::b_int64(idTokenValidity)),
                kvp("refreshTokenValidity", bsoncxx::types::b_int64(refreshTokenValidity)),
                kvp("created", bsoncxx::types::b_date(created)),
                kvp("modified", bsoncxx::types::b_date(modified)));
        return userPoolDocument;
    }

    void UserPoolClient::FromDocument(std::optional<bsoncxx::document::view> mResult) {

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

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::Cognito