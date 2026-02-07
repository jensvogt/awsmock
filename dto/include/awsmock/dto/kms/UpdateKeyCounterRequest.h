//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_UPDATE_KEY_COUNTER_REQUEST_H
#define AWSMOCK_DTO_KMS_UPDATE_KEY_COUNTER_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/KeyCounter.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Update KMS key request
     *
     * Example:
     * @code{.json}
     * {
     *   "keyCounter": object
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateKeyCounterRequest final : Common::BaseCounter<UpdateKeyCounterRequest> {

        /**
         * Key ID
         */
        KeyCounter keyCounter;

      private:

        friend UpdateKeyCounterRequest tag_invoke(boost::json::value_to_tag<UpdateKeyCounterRequest>, boost::json::value const &v) {
            UpdateKeyCounterRequest r;
            r.keyCounter = boost::json::value_to<KeyCounter>(v.at("keyCounter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateKeyCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyId", boost::json::value_from(obj.keyCounter)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_UPDATE_KEY_COUNTER_REQUEST_H
