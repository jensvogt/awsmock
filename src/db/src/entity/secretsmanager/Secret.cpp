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

        document versionIdStageDoc;
        for (const auto &key: versionIdsToStages.versions | std::views::keys) {
            array versionStateArray;
            for (const auto &stage: versionIdsToStages.versions.at(key)) {
                versionStateArray.append(stage);
            }
            versionIdStageDoc.append(kvp(key, versionStateArray));
        }

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
        secretDoc.append(kvp("createdDate", static_cast<bsoncxx::types::b_int64>(createdDate)));
        secretDoc.append(kvp("deletedDate", static_cast<bsoncxx::types::b_int64>(deletedDate)));
        secretDoc.append(kvp("lastAccessedDate", static_cast<bsoncxx::types::b_int64>(lastAccessedDate)));
        secretDoc.append(kvp("lastChangedDate", static_cast<bsoncxx::types::b_int64>(lastChangedDate)));
        secretDoc.append(kvp("lastRotatedDate", static_cast<bsoncxx::types::b_int64>(lastRotatedDate)));
        secretDoc.append(kvp("nextRotatedDate", static_cast<bsoncxx::types::b_int64>(nextRotatedDate)));
        secretDoc.append(kvp("rotationEnabled", rotationEnabled));
        secretDoc.append(kvp("rotationLambdaARN", rotationLambdaARN));
        secretDoc.append(kvp("rotationRules", rotationRulesDoc.extract()));
        secretDoc.append(kvp("versionIdsToStages", versionIdStageDoc.extract()));
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
            createdDate = Core::Bson::BsonUtils::GetLongValue(mResult, "createdDate");
            deletedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "deletedDate");
            lastAccessedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "lastAccessedDate");
            lastChangedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "lastChangedDate");
            lastRotatedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "lastRotatedDate");
            nextRotatedDate = Core::Bson::BsonUtils::GetLongValue(mResult, "nextRotatedDate");
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

            // Get version stages
            if (mResult.value().find("versionIdsToStages") != mResult.value().end()) {
                for (const view versionStagesView = mResult.value()["versionIdsToStages"].get_document().value; const auto &element: versionStagesView) {
                    std::string versionId = bsoncxx::string::to_string(element.key());
                    std::vector<std::string> stages;
                    for (const auto &stage: versionStagesView[versionId].get_array().value) {
                        stages.push_back(bsoncxx::string::to_string(stage.get_string().value));
                    }
                    versionIdsToStages.versions.emplace(versionId, stages);
                }
            }

        } catch (const bsoncxx::exception &exc) {
            log_error << "Exception: oid: " << oid << " error: " << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::SecretsManager
