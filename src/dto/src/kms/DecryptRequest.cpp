//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/DecryptRequest.h>

namespace AwsMock::Dto::KMS {

    void DecryptRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            keyId = Core::Bson::BsonUtils::GetStringValue(document, "KeyId");
            dryRun = Core::Bson::BsonUtils::GetBoolValue(document, "DryRun");
            ciphertext = Core::Bson::BsonUtils::GetStringValue(document, "CiphertextBlob");
            encryptionAlgorithm = EncryptionAlgorithmsFromString(Core::Bson::BsonUtils::GetStringValue(document, "EncryptionAlgorithm"));

            // Grant tokens
            if (document.view().find("GrantTokens") != document.view().end()) {
                for (const bsoncxx::array::view jsonTokenArray = document.view()["GrantTokens"].get_array().value; const auto &element: jsonTokenArray) {
                    grantTokens.emplace_back(element.get_string().value);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DecryptRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);
            Core::Bson::BsonUtils::SetBoolValue(document, "DryRun", dryRun);
            Core::Bson::BsonUtils::SetStringValue(document, "CiphertextBlob", ciphertext);
            Core::Bson::BsonUtils::SetStringValue(document, "EncryptionAlgorithm", EncryptionAlgorithmsToString(encryptionAlgorithm));

            if (!grantTokens.empty()) {
                array jsonArray;
                for (const auto &element: grantTokens) {
                    jsonArray.append(element);
                }
                document.append(kvp("GrantTokens", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
