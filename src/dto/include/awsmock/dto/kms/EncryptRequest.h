//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_ENCRYPT_REQUEST_H
#define AWSMOCK_DTO_KMS_ENCRYPT_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Encryption of supplied blob request
     *
     * Example:
     * @code{.json}
     * {
     *   "DryRun": boolean,
     *   "EncryptionAlgorithm": "string",
     *   "EncryptionContext": {
     *     "string" : "string"
     *   },
     *   "GrantTokens": [ "string" ],
     *   "KeyId": "string",
     *   "Plaintext": blob
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EncryptRequest final : Common::BaseCounter<EncryptRequest> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Plain text
         *
         * <p>
         * The plain text is Base64 encoded in case of AWS CLI, plain otherwise.
         * </p>
         */
        std::string plaintext;

        /**
         * Encryption algorithm
         */
        EncryptionAlgorithm encryptionAlgorithm;

        /**
         * Grant tokens
         */
        std::vector<std::string> grantTokens;

        /**
         * Dry run
         */
        bool dryRun = false;

      private:

        friend EncryptRequest tag_invoke(boost::json::value_to_tag<EncryptRequest>, boost::json::value const &v) {
            EncryptRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.plaintext = Core::Json::GetStringValue(v, "Plaintext");
            r.encryptionAlgorithm = EncryptionAlgorithmsFromString(Core::Json::GetStringValue(v, "EncryptionAlgorithm"));
            r.dryRun = Core::Json::GetBoolValue(v, "DryRun");
            if (Core::Json::AttributeExists(v, "GrantTokens")) {
                r.grantTokens = boost::json::value_to<std::vector<std::string>>(v.at("GrantTokens"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EncryptRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"Plaintext", obj.plaintext},
                    {"EncryptionAlgorithm", EncryptionAlgorithmsToString(obj.encryptionAlgorithm)},
                    {"GrantTokens", boost::json::value_from(obj.grantTokens)},
                    {"DryRun", obj.dryRun},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_ENCRYPT_REQUEST_H
