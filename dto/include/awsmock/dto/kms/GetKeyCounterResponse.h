//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_GET_KEY_COUNTER_RESPONSE_H
#define AWSMOCK_DTO_KMS_GET_KEY_COUNTER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "model/KeyCounter.h"


#include <awsmock/dto/kms/model/Key.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Get KMS key response
     *
     * Example:
     * @code{.json}
     * {
     *   "key": "object"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetKeyCounterResponse final : Common::BaseCounter<GetKeyCounterResponse> {

        /**
         * Key metadata
         */
        KeyCounter key;

      private:

        friend GetKeyCounterResponse tag_invoke(boost::json::value_to_tag<GetKeyCounterResponse>, boost::json::value const &v) {
            GetKeyCounterResponse r;
            r.key = boost::json::value_to<KeyCounter>(v.at("keyCounter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetKeyCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyCounter", boost::json::value_from(obj.key)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_GET_KEY_COUNTER_RESPONSE_H
