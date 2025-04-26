//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/DecryptResponse.h>

namespace AwsMock::Dto::KMS {

    void DecryptResponse::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            keyId = Core::Bson::BsonUtils::GetStringValue(document, "KeyId");
            plaintext = Core::Bson::BsonUtils::GetStringValue(document, "Plaintext");
            encryptionAlgorithm = EncryptionAlgorithmsFromString(Core::Bson::BsonUtils::GetStringValue(document, "EncryptionAlgorithm"));

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DecryptResponse::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);
            Core::Bson::BsonUtils::SetStringValue(document, "Plaintext", plaintext);
            Core::Bson::BsonUtils::SetStringValue(document, "EncryptionAlgorithm", EncryptionAlgorithmsToString(encryptionAlgorithm));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
