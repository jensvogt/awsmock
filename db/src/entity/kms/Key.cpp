//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/kms/Key.h>

namespace AwsMock::Database::Entity::KMS {

    view_or_value<view, value> Key::ToDocument() const {

        try {
            document keyDoc;
            keyDoc.append(
                    kvp("region", region),
                    kvp("arn", arn),
                    kvp("keyId", keyId),
                    kvp("keyUsage", keyUsage),
                    kvp("keySpec", keySpec),
                    kvp("keyState", keyState),
                    kvp("origin", origin),
                    kvp("aes256Key", aes256Key),
                    kvp("aes256Iv", aes256Iv),
                    kvp("hmac224Key", hmac224Key),
                    kvp("hmac256Key", hmac256Key),
                    kvp("hmac384Key", hmac384Key),
                    kvp("hmac512Key", hmac512Key),
                    kvp("rsaPrivateKey", rsaPrivateKey),
                    kvp("rsaPublicKey", rsaPublicKey),
                    kvp("pendingWindowInDays", bsoncxx::types::b_int64(pendingWindowInDays)),
                    kvp("description", description),
                    kvp("created", bsoncxx::types::b_date(created)),
                    kvp("modified", bsoncxx::types::b_date(modified)));

            // Scheduled deletion
            if (scheduledDeletion.time_since_epoch().count() > 0) {
                keyDoc.append(kvp("scheduledDeletion", bsoncxx::types::b_date(scheduledDeletion)));
            } else {
                keyDoc.append(kvp("scheduledDeletion", bsoncxx::types::b_null()));
            }

            // Tags
            if (!tags.empty()) {
                auto tagsDoc = document{};
                for (const auto &[fst, snd]: tags) {
                    tagsDoc.append(kvp(fst, snd));
                }
                keyDoc.append(kvp("tags", tagsDoc));
            }
            return keyDoc.extract();

        } catch (std::exception &e) {
            log_error << e.what();
            throw Core::DatabaseException(e.what());
        }
    }

    void Key::FromDocument(const std::optional<view> &mResult) {

        try {
            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
            keyId = Core::Bson::BsonUtils::GetStringValue(mResult, "keyId");
            keyUsage = Core::Bson::BsonUtils::GetStringValue(mResult, "keyUsage");
            keySpec = Core::Bson::BsonUtils::GetStringValue(mResult, "keySpec");
            keyState = Core::Bson::BsonUtils::GetStringValue(mResult, "keyState");
            origin = Core::Bson::BsonUtils::GetStringValue(mResult, "origin");
            aes256Key = Core::Bson::BsonUtils::GetStringValue(mResult, "aes256Key");
            aes256Iv = Core::Bson::BsonUtils::GetStringValue(mResult, "aes256Iv");
            hmac224Key = Core::Bson::BsonUtils::GetStringValue(mResult, "hmac224Key");
            hmac256Key = Core::Bson::BsonUtils::GetStringValue(mResult, "hmac256Key");
            hmac384Key = Core::Bson::BsonUtils::GetStringValue(mResult, "hmac384Key");
            hmac512Key = Core::Bson::BsonUtils::GetStringValue(mResult, "hmac512Key");
            rsaPrivateKey = Core::Bson::BsonUtils::GetStringValue(mResult, "rsaPrivateKey");
            rsaPublicKey = Core::Bson::BsonUtils::GetStringValue(mResult, "rsaPublicKey");
            pendingWindowInDays = Core::Bson::BsonUtils::GetIntValue(mResult, "pendingWindowInDays");
            scheduledDeletion = Core::Bson::BsonUtils::GetDateValue(mResult, "scheduledDeletion");
            description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Get tags
            if (mResult.value().find("tags") != mResult.value().end()) {
                for (const view tagsView = mResult.value()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                    std::string key = bsoncxx::string::to_string(tagElement.key());
                    std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                    tags.emplace(key, value);
                }
            }

        } catch (std::exception &e) {
            log_error << e.what();
            throw Core::DatabaseException(e.what());
        }
    }

    std::string Key::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Key::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Key &t) {
        os << "Key=" << bsoncxx::to_json(t.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::KMS
