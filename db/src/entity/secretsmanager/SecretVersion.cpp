//
// Created by vogje01 on 24/09/2023.
//

#include <awsmock/entity/secretsmanager/SecretVersion.h>

namespace AwsMock::Database::Entity::SecretsManager {

    view_or_value<view, value> SecretVersion::ToDocument() const {

        array stagesArray;
        for (const auto &stage: stages) {
            stagesArray.append(stage);
        }

        document secretVersionDoc;
        secretVersionDoc.append(kvp("secretString", secretString));
        secretVersionDoc.append(kvp("secretBinary", secretBinary));
        secretVersionDoc.append(kvp("created", bsoncxx::types::b_date(created)));
        secretVersionDoc.append(kvp("lastAccessed", bsoncxx::types::b_date(lastAccessed)));
        secretVersionDoc.append(kvp("stages", stagesArray.extract()));

        return secretVersionDoc.extract();
    }

    void SecretVersion::FromDocument(const std::optional<view> &mResult) {

        try {
            secretString = Core::Bson::BsonUtils::GetStringValue(mResult, "secretString");
            secretBinary = Core::Bson::BsonUtils::GetStringValue(mResult, "secretBinary");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            lastAccessed = Core::Bson::BsonUtils::GetDateValue(mResult, "lastAccessed");

            // Get stages
            if (mResult.value().find("stages") != mResult.value().end()) {
                for (const view versionsView = mResult.value()["stages"].get_array().value; const auto &element: versionsView) {
                    stages.emplace_back(Core::Bson::BsonUtils::GetStringValue(element));
                }
            }

        } catch (const bsoncxx::exception &exc) {
            log_error << "Exception: oid: " << oid << " error: " << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::SecretsManager
