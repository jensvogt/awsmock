//
// Created by vogje01 on 4/26/24.
//
#ifndef AWSMOCK_DTO_KMS_KEY_USAGE_H
#define AWSMOCK_DTO_KMS_KEY_USAGE_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS key usage
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class KeyUsage {
        SIGN_VERIFY,
        ENCRYPT_DECRYPT,
        GENERATE_VERIFY_MAC
    };

    static std::map<KeyUsage, std::string> KeyUsageNames{
            {KeyUsage::SIGN_VERIFY, "SIGN_VERIFY"},
            {KeyUsage::ENCRYPT_DECRYPT, "ENCRYPT_DECRYPT"},
            {KeyUsage::GENERATE_VERIFY_MAC, "GENERATE_VERIFY_MAC"}};

    [[maybe_unused]] static std::string KeyUsageToString(const KeyUsage &keyUsage) {
        return KeyUsageNames[keyUsage];
    }

    [[maybe_unused]] static KeyUsage KeyUsageFromString(const std::string &keyUsage) {
        for (auto &[fst, snd]: KeyUsageNames) {
            if (snd == keyUsage) {
                return fst;
            }
        }
        return KeyUsage::ENCRYPT_DECRYPT;
    }

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_KEY_USAGE_H
