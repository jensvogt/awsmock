//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/apigateway/Resource.h>

namespace Awsmock::Database::Entity::ApiGateway {

    view_or_value<view, value> Resource::ToDocument() const {

        document keyDocument;
        keyDocument.append(kvp("region", region));
        keyDocument.append(kvp("id", id));
        keyDocument.append(kvp("parentId", parentId));
        keyDocument.append(kvp("path", path));
        keyDocument.append(kvp("pathPart", pathPart));
        keyDocument.append(kvp("url", url));
        keyDocument.append(kvp("created", bsoncxx::types::b_date(created)));
        keyDocument.append(kvp("modified", bsoncxx::types::b_date(modified)));
        return keyDocument.extract();
    }

    void Resource::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        id = Core::Bson::BsonUtils::GetStringValue(mResult, "id");
        parentId = Core::Bson::BsonUtils::GetStringValue(mResult, "parentId");
        path = Core::Bson::BsonUtils::GetStringValue(mResult, "path");
        pathPart = Core::Bson::BsonUtils::GetStringValue(mResult, "pathPart");
        url = Core::Bson::BsonUtils::GetStringValue(mResult, "url");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");
    }

}// namespace Awsmock::Database::Entity::ApiGateway