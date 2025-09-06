//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/apigateway/Rest.h>

namespace AwsMock::Database::Entity::ApiGateway {

    view_or_value<view, value> Rest::ToDocument() const {

        document keyDocument;
        keyDocument.append(kvp("region", region));
        keyDocument.append(kvp("name", name));
        keyDocument.append(kvp("description", description));
        keyDocument.append(kvp("endpointUrl", endpointUrl));
        keyDocument.append(kvp("apiKeySource", apiKeySource));
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

    void Rest::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
        endpointUrl = Core::Bson::BsonUtils::GetStringValue(mResult, "endpointUrl");
        apiKeySource = Core::Bson::BsonUtils::GetStringValue(mResult, "apiKeySource");
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