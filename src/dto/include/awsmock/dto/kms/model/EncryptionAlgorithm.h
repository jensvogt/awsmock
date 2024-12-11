//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_KMS_ENCRYPTION_ALGORITHMS_H
#define AWSMOCK_DTO_KMS_ENCRYPTION_ALGORITHMS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::KMS {

    /**
     * KMS key expiration model
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class EncryptionAlgorithm {
        SYMMETRIC_DEFAULT,
        RSAES_OAEP_SHA_1,
        RSAES_OAEP_SHA_256,
        SM2PKE
    };

    static std::map<EncryptionAlgorithm, std::string> EncryptionAlgorithmsNames{
            {EncryptionAlgorithm::SYMMETRIC_DEFAULT, "SYMMETRIC_DEFAULT"},
            {EncryptionAlgorithm::RSAES_OAEP_SHA_1, "RSAES_OAEP_SHA_1"},
            {EncryptionAlgorithm::RSAES_OAEP_SHA_256, "RSAES_OAEP_SHA_256"},
            {EncryptionAlgorithm::SM2PKE, "SM2PKE"}};

    [[maybe_unused]] static std::string EncryptionAlgorithmsToString(EncryptionAlgorithm encryptionAlgorithm) {
        return EncryptionAlgorithmsNames[encryptionAlgorithm];
    }

    [[maybe_unused]] static EncryptionAlgorithm EncryptionAlgorithmsFromString(const std::string &encryptionAlgorithm) {
        for (auto &it: EncryptionAlgorithmsNames) {
            if (it.second == encryptionAlgorithm) {
                return it.first;
            }
        }
        return EncryptionAlgorithm::SYMMETRIC_DEFAULT;
    }

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_ENCRYPTION_ALGORITHMS_H
