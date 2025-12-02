//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_KMS_MASTER_KEY_SPEC_H
#define AWSMOCK_DTO_KMS_MASTER_KEY_SPEC_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS master key specification
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class KeySpec {
        RSA_2048,
        RSA_3072,
        RSA_4096,
        ECC_NIST_P256,
        ECC_NIST_P384,
        ECC_NIST_P521,
        ECC_SECG_P256K1,
        SYMMETRIC_DEFAULT,
        HMAC_224,
        HMAC_256,
        HMAC_384,
        HMAC_512
    };

    static std::map<KeySpec, std::string> KeySpecNames{
            {KeySpec::RSA_2048, "RSA_2048"},
            {KeySpec::RSA_3072, "RSA_3072"},
            {KeySpec::RSA_4096, "RSA_4096"},
            {KeySpec::ECC_NIST_P256, "ECC_NIST_P256"},
            {KeySpec::ECC_NIST_P384, "ECC_NIST_P384"},
            {KeySpec::ECC_NIST_P521, "ECC_NIST_P521"},
            {KeySpec::ECC_SECG_P256K1, "ECC_SECG_P256K1"},
            {KeySpec::SYMMETRIC_DEFAULT, "SYMMETRIC_DEFAULT"},
            {KeySpec::HMAC_224, "HMAC_224"},
            {KeySpec::HMAC_256, "HMAC_256"},
            {KeySpec::HMAC_384, "HMAC_384"},
            {KeySpec::HMAC_512, "HMAC_512"}};

    [[maybe_unused]] static std::string KeySpecToString(const KeySpec &keySpec) {
        return KeySpecNames[keySpec];
    }

    [[maybe_unused]] static KeySpec KeySpecFromString(const std::string &keySpec) {
        for (auto &[fst, snd]: KeySpecNames) {
            if (snd == keySpec) {
                return fst;
            }
        }
        return KeySpec::SYMMETRIC_DEFAULT;
    }

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_MASTER_KEY_SPEC_H
