//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/apigateway/ResourceMethod.h>

namespace Awsmock::Database::Entity::ApiGateway {

    view_or_value<view, value> ResourceMethod::ToDocument() const {

        document keyDocument;
        keyDocument.append(kvp("region", region));
        keyDocument.append(kvp("apiKeyRequired", apiKeyRequired));
        keyDocument.append(kvp("httpMethod", httpMethod));
        keyDocument.append(kvp("integrationType", integrationType));
        keyDocument.append(kvp("integrationUri", integrationUri));
        keyDocument.append(kvp("integrationHttpMethod", integrationHttpMethod));
        return keyDocument.extract();
    }

    void ResourceMethod::FromDocument(const std::optional<view> &mResult) {

        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        apiKeyRequired = Core::Bson::BsonUtils::GetBoolValue(mResult, "apiKeyRequired");
        httpMethod = Core::Bson::BsonUtils::GetStringValue(mResult, "httpMethod");
        integrationType = Core::Bson::BsonUtils::GetStringValue(mResult, "integrationType");
        integrationUri = Core::Bson::BsonUtils::GetStringValue(mResult, "integrationUri");
        integrationHttpMethod = Core::Bson::BsonUtils::GetStringValue(mResult, "integrationHttpMethod");
    }

}// namespace Awsmock::Database::Entity::ApiGateway