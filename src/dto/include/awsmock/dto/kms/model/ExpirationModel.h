//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_KMS_EXPIRATION_MODEL_H
#define AWSMOCK_DTO_KMS_EXPIRATION_MODEL_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS key expiration model
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ExpirationModel {
        KEY_MATERIAL_EXPIRES,
        KEY_MATERIAL_DOES_NOT_EXPIRE
    };

    static std::map<ExpirationModel, std::string> ExpirationModelNames{
            {ExpirationModel::KEY_MATERIAL_EXPIRES, "KEY_MATERIAL_EXPIRES"},
            {ExpirationModel::KEY_MATERIAL_DOES_NOT_EXPIRE, "KEY_MATERIAL_DOES_NOT_EXPIRE"}};

    [[maybe_unused]] static std::string ExpirationModelToString(const ExpirationModel &expirationModel) {
        return ExpirationModelNames[expirationModel];
    }

    [[maybe_unused]] static ExpirationModel ExpirationModelFromString(const std::string &expirationModel) {
        for (auto &[fst, snd]: ExpirationModelNames) {
            if (snd == expirationModel) {
                return fst;
            }
        }
        return ExpirationModel::KEY_MATERIAL_DOES_NOT_EXPIRE;
    }

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_EXPIRATION_MODEL_H
