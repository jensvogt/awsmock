//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/ssm/Parameter.h>

namespace AwsMock::Database::Entity::SSM {

    view_or_value<view, value> Parameter::ToDocument() const {

        auto tagsDoc = document{};
        if (!tags.empty()) {
            for (const auto &[fst, snd]: tags) {
                tagsDoc.append(kvp(fst, snd));
            }
        }

        document parameterDocument;
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "region", region);
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "name", parameterName);
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "value", parameterValue);
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "type", type);
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "description", description);
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "tier", tier);
        Core::Bson::BsonUtils::SetIntValue(parameterDocument, "version", version);
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "arn", arn);
        Core::Bson::BsonUtils::SetStringValue(parameterDocument, "kmsKeyArn", kmsKeyArn);
        Core::Bson::BsonUtils::SetDocumentValue(parameterDocument, "tags", tagsDoc);
        Core::Bson::BsonUtils::SetDateValue(parameterDocument, "created", created);
        Core::Bson::BsonUtils::SetDateValue(parameterDocument, "modified", modified);
        return parameterDocument.extract();
    }

    auto Parameter::FromDocument(const std::optional<view> &mResult) -> void {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        parameterName = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        parameterValue = Core::Bson::BsonUtils::GetStringValue(mResult, "value");
        type = Core::Bson::BsonUtils::GetStringValue(mResult, "type");
        description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
        tier = Core::Bson::BsonUtils::GetStringValue(mResult, "tier");
        version = Core::Bson::BsonUtils::GetIntValue(mResult, "version");
        arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
        kmsKeyArn = Core::Bson::BsonUtils::GetStringValue(mResult, "kmsKeyArn");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Get tags
        if (mResult.value().find("tags") != mResult.value().end()) {
            tags.clear();
            for (const view tagsView = mResult.value()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                std::string key = bsoncxx::string::to_string(tagElement.key());
                std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                tags.emplace(key, value);
            }
        }
    }

    std::string Parameter::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Parameter::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Parameter &u) {
        os << "Parameter=" << u.ToJson();
        return os;
    }
}// namespace AwsMock::Database::Entity::SSM