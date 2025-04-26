//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/EncryptRequest.h>

namespace AwsMock::Dto::KMS {

    void EncryptRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            keyId = Core::Bson::BsonUtils::GetStringValue(document, "KeyId");
            dryRun = Core::Bson::BsonUtils::GetBoolValue(document, "DryRun");
            plainText = Core::Bson::BsonUtils::GetStringValue(document, "Plaintext");
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

    std::string EncryptRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);
            Core::Bson::BsonUtils::SetBoolValue(document, "DryRun", dryRun);
            Core::Bson::BsonUtils::SetStringValue(document, "Plaintext", plainText);
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
