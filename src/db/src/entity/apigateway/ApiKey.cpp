//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/apigateway/ApiKey.h>

namespace AwsMock::Database::Entity::ApiGateway {

    view_or_value<view, value> ApiKey::ToDocument() const {

        document keyDocument;
        keyDocument.append(kvp("region", region));
        keyDocument.append(kvp("id", id));
        keyDocument.append(kvp("name", name));
        keyDocument.append(kvp("customerId", customerId));
        keyDocument.append(kvp("description", description));
        keyDocument.append(kvp("enabled", enabled));
        keyDocument.append(kvp("generateDistinct", generateDistinct));
        keyDocument.append(kvp("value", keyValue));
        keyDocument.append(kvp("created", bsoncxx::types::b_date(created)));
        keyDocument.append(kvp("modified", bsoncxx::types::b_date(modified)));

        // Tags
        if (!tags.empty()) {
            document tagsDoc;
            for (const auto &[fst, snd]: tags) {
                tagsDoc.append(kvp(fst, snd));
            }
            keyDocument.append(kvp("tags", tagsDoc));
        }
        return keyDocument.extract();
    }

    void ApiKey::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        id = Core::Bson::BsonUtils::GetStringValue(mResult, "id");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        customerId = Core::Bson::BsonUtils::GetStringValue(mResult, "customerId");
        description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
        enabled = Core::Bson::BsonUtils::GetBoolValue(mResult, "enabled");
        generateDistinct = Core::Bson::BsonUtils::GetBoolValue(mResult, "generateDistinct");
        keyValue = Core::Bson::BsonUtils::GetStringValue(mResult, "value");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Tags
        if (mResult.value().find("tags") != mResult.value().end()) {
            tags.clear();
            for (const view tagsObject = mResult.value()["tags"].get_document().value; const auto &t: tagsObject) {
                const std::string key = bsoncxx::string::to_string(t.key());
                const std::string value = bsoncxx::string::to_string(tagsObject[key].get_string().value);
                tags[key] = value;
            }
        }
    }

}// namespace AwsMock::Database::Entity::ApiGateway