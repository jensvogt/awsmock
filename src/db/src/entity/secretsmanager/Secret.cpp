//
// Created by vogje01 on 24/09/2023.
//

#include <awsmock/entity/secretsmanager/Secret.h>

namespace AwsMock::Database::Entity::SecretsManager {

    view_or_value<view, value> Secret::ToDocument() const {

        document rotationRulesDoc;
        rotationRulesDoc.append(kvp("automaticallyAfterDays", static_cast<bsoncxx::types::b_int64>(rotationRules.automaticallyAfterDays)));
        rotationRulesDoc.append(kvp("duration", rotationRules.duration));
        rotationRulesDoc.append(kvp("scheduleExpression", rotationRules.scheduleExpression));

        document versionsDoc;
        for (const auto &[fst, snd]: versions) {
            versionsDoc.append(kvp(fst, snd.ToDocument()));
        }

        document secretDoc;
        secretDoc.append(kvp("region", region));
        secretDoc.append(kvp("name", name));
        secretDoc.append(kvp("arn", arn));
        secretDoc.append(kvp("secretId", secretId));
        secretDoc.append(kvp("kmsKeyId", kmsKeyId));
        secretDoc.append(kvp("versions", versionsDoc.extract()));
        secretDoc.append(kvp("description", description));
        secretDoc.append(kvp("owningService", owningService));
        secretDoc.append(kvp("primaryRegion", primaryRegion));
        secretDoc.append(kvp("createdDate", bsoncxx::types::b_date(createdDate)));
        secretDoc.append(kvp("deletedDate", bsoncxx::types::b_date(deletedDate)));
        secretDoc.append(kvp("lastAccessedDate", bsoncxx::types::b_date(lastAccessedDate)));
        secretDoc.append(kvp("lastChangedDate", bsoncxx::types::b_date(lastChangedDate)));
        secretDoc.append(kvp("lastRotatedDate", bsoncxx::types::b_date(lastRotatedDate)));
        secretDoc.append(kvp("nextRotatedDate", bsoncxx::types::b_date(nextRotatedDate)));
        secretDoc.append(kvp("rotationEnabled", rotationEnabled));
        secretDoc.append(kvp("rotationLambdaARN", rotationLambdaARN));
        secretDoc.append(kvp("rotationRules", rotationRulesDoc.extract()));
        secretDoc.append(kvp("created", bsoncxx::types::b_date(created)));
        secretDoc.append(kvp("modified", bsoncxx::types::b_date(modified)));

        return secretDoc.extract();
    }

    void Secret::FromDocument(const std::optional<view> &mResult) {

        try {
            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
            arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
            secretId = Core::Bson::BsonUtils::GetStringValue(mResult, "secretId");
            kmsKeyId = Core::Bson::BsonUtils::GetStringValue(mResult, "kmsKeyId");
            description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
            owningService = Core::Bson::BsonUtils::GetStringValue(mResult, "owningService");
            primaryRegion = Core::Bson::BsonUtils::GetStringValue(mResult, "primaryRegion");
            createdDate = Core::Bson::BsonUtils::GetDateValue(mResult, "createdDate");
            deletedDate = Core::Bson::BsonUtils::GetDateValue(mResult, "deletedDate");
            lastAccessedDate = Core::Bson::BsonUtils::GetDateValue(mResult, "lastAccessedDate");
            lastChangedDate = Core::Bson::BsonUtils::GetDateValue(mResult, "lastChangedDate");
            lastRotatedDate = Core::Bson::BsonUtils::GetDateValue(mResult, "lastRotatedDate");
            nextRotatedDate = Core::Bson::BsonUtils::GetDateValue(mResult, "nextRotatedDate");
            rotationEnabled = Core::Bson::BsonUtils::GetBoolValue(mResult, "rotationEnabled");
            rotationLambdaARN = Core::Bson::BsonUtils::GetStringValue(mResult, "rotationLambdaARN");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Get versions
            if (mResult.value().find("versions") != mResult.value().end()) {
                for (const view versionsView = mResult.value()["versions"].get_document().value; const auto &element: versionsView) {
                    std::string versionId = bsoncxx::string::to_string(element.key());
                    SecretVersion version;
                    version.FromDocument(element.get_document().value);
                    versions[versionId] = version;
                }
            }

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

}// namespace AwsMock::Database::Entity::SecretsManager
