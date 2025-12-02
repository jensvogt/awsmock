//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_KMS_ORIGIN_H
#define AWSMOCK_DTO_KMS_ORIGIN_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS key origin
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class Origin {
        AWS_KMS,
        EXTERNAL,
        AWS_CLOUDHSM,
        EXTERNAL_KEY_STORE
    };

    static std::map<Origin, std::string> OriginNames{
            {Origin::AWS_KMS, "AWS_KMS"},
            {Origin::EXTERNAL, "EXTERNAL"},
            {Origin::AWS_CLOUDHSM, "AWS_CLOUDHSM"},
            {Origin::EXTERNAL_KEY_STORE, "EXTERNAL_KEY_STORE"}};

    [[maybe_unused]] static std::string OriginToString(const Origin &keySpec) {
        return OriginNames[keySpec];
    }

    [[maybe_unused]] static Origin OriginFromString(const std::string &origin) {
        for (auto &[fst, snd]: OriginNames) {
            if (snd == origin) {
                return fst;
            }
        }
        return Origin::AWS_KMS;
    }

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_ORIGIN_H
