//
// Created by vogje01 on 4/26/24.
//

#ifndef AWSMOCK_DTO_KMS_KEY_COUNTER_H
#define AWSMOCK_DTO_KMS_KEY_COUNTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>
#include <awsmock/dto/kms/model/KeySpec.h>
#include <awsmock/dto/kms/model/KeyState.h>
#include <awsmock/dto/kms/model/KeyUsage.h>
#include <awsmock/dto/kms/model/Origin.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS key counter
     *
     * Example:
     * @code{.json}
     * {
     *   "keyId": "string",
     *   "keyArn": "string",
     *   "keySpec": "string",
     *   "keyState": "string",
     *   "keyUsage": "string",
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct KeyCounter final : Common::BaseCounter<KeyCounter> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Arn
         */
        std::string arn;

        /**
         * Key specification
         */
        KeySpec keySpec = KeySpec::SYMMETRIC_DEFAULT;

        /**
         * Key usage
         */
        KeyUsage keyUsage = KeyUsage::ENCRYPT_DECRYPT;

        /**
         * Key state
         */
        KeyState keyState = KeyState::UNAVAILABLE;

        /**
         * Multi region
         */
        bool multiRegion = false;

        /**
         * Description
         */
        std::string description;

        /**
         * Creation date
         */
        system_clock::time_point created;

        /**
         * Modification date
         */
        system_clock::time_point modified;

      private:

        friend KeyCounter tag_invoke(boost::json::value_to_tag<KeyCounter>, boost::json::value const &v) {
            KeyCounter r;
            r.keyId = Core::Json::GetStringValue(v, "keyId");
            r.arn = Core::Json::GetStringValue(v, "keyArn");
            r.keySpec = KeySpecFromString(Core::Json::GetStringValue(v, "keySpec"));
            r.keyUsage = KeyUsageFromString(Core::Json::GetStringValue(v, "keyUsage"));
            r.keyState = KeyStateFromString(Core::Json::GetStringValue(v, "keyState"));
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, KeyCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyId", obj.keyId},
                    {"keyArn", obj.arn},
                    {"keySpec", KeySpecToString(obj.keySpec)},
                    {"keyUsage", KeyUsageToString(obj.keyUsage)},
                    {"keyState", KeyStateToString(obj.keyState)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_KEY_COUNTER_H
