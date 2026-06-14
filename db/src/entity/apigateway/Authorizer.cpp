//
// Created by vogje01 on 6/13/26.
//

#include <awsmock/entity/apigateway/Authorizer.h>

namespace Awsmock::Database::Entity::ApiGateway {

    view_or_value<view, value> Authorizer::ToDocument() const {

        document doc;
        doc.append(kvp("id", id));
        doc.append(kvp("name", name));
        doc.append(kvp("type", type));
        doc.append(kvp("authType", authType));
        doc.append(kvp("authorizerUri", authorizerUri));
        doc.append(kvp("identitySource", identitySource));
        doc.append(kvp("authorizerResultTtlInSeconds", authorizerResultTtlInSeconds));
        doc.append(kvp("created", bsoncxx::types::b_date(created)));
        doc.append(kvp("modified", bsoncxx::types::b_date(modified)));
        return doc.extract();
    }

    void Authorizer::FromDocument(const std::optional<view> &mResult) {

        id = Core::Bson::BsonUtils::GetStringValue(mResult, "id");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        type = Core::Bson::BsonUtils::GetStringValue(mResult, "type");
        authType = Core::Bson::BsonUtils::GetStringValue(mResult, "authType");
        authorizerUri = Core::Bson::BsonUtils::GetStringValue(mResult, "authorizerUri");
        identitySource = Core::Bson::BsonUtils::GetStringValue(mResult, "identitySource");
        authorizerResultTtlInSeconds = Core::Bson::BsonUtils::GetLongValue(mResult, "authorizerResultTtlInSeconds");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");
    }

}// namespace Awsmock::Database::Entity::ApiGateway
