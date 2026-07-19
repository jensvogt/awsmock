//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct GenerateDataKeyResponse final : Common::BaseCounter<GenerateDataKeyResponse> {

        std::string keyId;
        std::string ciphertextBlob;
        std::string plaintext;

        [[nodiscard]] std::string ToJson() const {
            try {
                document doc;
                Core::Bson::BsonUtils::SetStringValue(doc, "KeyId", keyId);
                Core::Bson::BsonUtils::SetStringValue(doc, "CiphertextBlob", ciphertextBlob);
                Core::Bson::BsonUtils::SetStringValue(doc, "Plaintext", plaintext);
                return Core::Bson::BsonUtils::ToJsonString(doc);
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:
        mutable logger_t _logger{boost::log::keywords::channel = "KMS"};
    };

}// namespace Awsmock::Dto::KMS
