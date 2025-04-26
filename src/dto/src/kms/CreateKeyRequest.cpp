//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/CreateKeyRequest.h>

namespace AwsMock::Dto::KMS {

    void CreateKeyRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            bypassPolicyLockoutSafetyCheck = Core::Bson::BsonUtils::GetBoolValue(document, "BypassPolicyLockoutSafetyCheck");
            keySpec = KeySpecFromString(Core::Bson::BsonUtils::GetStringValue(document, "KeySpec"));
            keyUsage = KeyUsageFromString(Core::Bson::BsonUtils::GetStringValue(document, "KeyUsage"));
            customKeyStoreId = Core::Bson::BsonUtils::GetStringValue(document, "CustomKeyStoreId");
            description = Core::Bson::BsonUtils::GetStringValue(document, "Description");
            origin = Core::Bson::BsonUtils::GetStringValue(document, "Origin");
            policy = Core::Bson::BsonUtils::GetStringValue(document, "Policy");
            xksKeyId = Core::Bson::BsonUtils::GetStringValue(document, "XksKeyId");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateKeyRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetBoolValue(document, "BypassPolicyLockoutSafetyCheck", bypassPolicyLockoutSafetyCheck);
            Core::Bson::BsonUtils::SetStringValue(document, "KeySpec", KeySpecToString(keySpec));
            Core::Bson::BsonUtils::SetStringValue(document, "KeyUsage", KeyUsageToString(keyUsage));
            Core::Bson::BsonUtils::SetStringValue(document, "CustomKeyStoreId", customKeyStoreId);
            Core::Bson::BsonUtils::SetStringValue(document, "Description", description);
            Core::Bson::BsonUtils::SetStringValue(document, "Origin", origin);
            Core::Bson::BsonUtils::SetStringValue(document, "Policy", policy);
            Core::Bson::BsonUtils::SetStringValue(document, "XksKeyId", xksKeyId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
