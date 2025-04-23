//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/EncryptResponse.h>

namespace AwsMock::Dto::KMS {

    void EncryptResponse::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            keyId = Core::Bson::BsonUtils::GetStringValue(document, "KeyId");
            ciphertext = Core::Bson::BsonUtils::GetStringValue(document, "CiphertextBlob");
            encryptionAlgorithm = EncryptionAlgorithmsFromString(Core::Bson::BsonUtils::GetStringValue(document, "EncryptionAlgorithm"));

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string EncryptResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);
            Core::Bson::BsonUtils::SetStringValue(document, "CiphertextBlob", ciphertext);
            Core::Bson::BsonUtils::SetStringValue(document, "EncryptionAlgorithm", EncryptionAlgorithmsToString(encryptionAlgorithm));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
