//
// Created by vogje01 on 24/09/2023.
//

#include <awsmock/entity/secretsmanager/Secret.h>

namespace AwsMock::Database::Entity::SecretsManager {

    view_or_value<view, value> Secret::ToDocument() const {

        view_or_value<view, value> rotationRulesDoc = make_document(
                kvp("automaticallyAfterDays", static_cast<bsoncxx::types::b_int64>(rotationRules.automaticallyAfterDays)),
                kvp("duration", rotationRules.duration),
                kvp("scheduleExpression", rotationRules.scheduleExpression));

        view_or_value<view, value> secretDoc = make_document(
                kvp("region", region),
                kvp("name", name),
                kvp("arn", arn),
                kvp("secretId", secretId),
                kvp("kmsKeyId", kmsKeyId),
                kvp("versionId", versionId),
                kvp("secretString", secretString),
                kvp("secretBinary", secretBinary),
                kvp("description", description),
                kvp("owningService", owningService),
                kvp("primaryRegion", primaryRegion),
                kvp("createdDate", static_cast<bsoncxx::types::b_int64>(createdDate)),
                kvp("deletedDate", static_cast<bsoncxx::types::b_int64>(deletedDate)),
                kvp("lastAccessedDate", static_cast<bsoncxx::types::b_int64>(lastAccessedDate)),
                kvp("lastChangedDate", static_cast<bsoncxx::types::b_int64>(lastChangedDate)),
                kvp("lastRotatedDate", static_cast<bsoncxx::types::b_int64>(lastRotatedDate)),
                kvp("nextRotatedDate", static_cast<bsoncxx::types::b_int64>(nextRotatedDate)),
                kvp("rotationEnabled", rotationEnabled),
                kvp("rotationLambdaARN", rotationLambdaARN),
                kvp("rotationRules", rotationRulesDoc),
                kvp("created", bsoncxx::types::b_date(created)),
                kvp("modified", bsoncxx::types::b_date(modified)));

        return secretDoc;
    }

    void Secret::FromDocument(const std::optional<view> &mResult) {

        try {
            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
            arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
            secretId = Core::Bson::BsonUtils::GetStringValue(mResult, "secretId");
            kmsKeyId = Core::Bson::BsonUtils::GetStringValue(mResult, "kmsKeyId");
            versionId = Core::Bson::BsonUtils::GetStringValue(mResult, "versionId");
            secretString = Core::Bson::BsonUtils::GetStringValue(mResult, "secretString");
            secretBinary = Core::Bson::BsonUtils::GetStringValue(mResult, "secretBinary");
            description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
            owningService = Core::Bson::BsonUtils::GetStringValue(mResult, "owningService");
            primaryRegion = Core::Bson::BsonUtils::GetStringValue(mResult, "primaryRegion");
            createdDate = Core::Bson::BsonUtils::GetLongValue(mResult, "createdDate");
            deletedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "deletedDate");
            lastAccessedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "lastAccessedDate");
            lastChangedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "lastChangedDate");
            lastRotatedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "lastRotatedDate");
            nextRotatedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "nextRotatedDate");
            rotationEnabled = Core::Bson::BsonUtils::GetLongValue(mResult, "rotationEnabled");
            rotationLambdaARN = Core::Bson::BsonUtils::GetStringValue(mResult, "rotationLambdaARN");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Get rotation rules
            if (mResult.value().find("rotationRules") != mResult.value().end()) {
                const view rotationView = mResult.value()["rotationRules"].get_document().value;
                rotationRules.automaticallyAfterDays = Core::Bson::BsonUtils::GetLongValue(rotationView, "automaticallyAfterDays");
                rotationRules.duration = Core::Bson::BsonUtils::GetStringValue(rotationView, "duration");
                rotationRules.scheduleExpression = Core::Bson::BsonUtils::GetStringValue(rotationView, "scheduleExpression");
            }
        } catch (const bsoncxx::exception &exc) {
            log_error << "Exception: oid: " << oid << " error: " << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Secret::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Secret::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Secret &s) {
        os << "Secret=" << to_json(s.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SecretsManager
