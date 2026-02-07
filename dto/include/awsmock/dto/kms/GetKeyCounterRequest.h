//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_GET_KEY_COUNTER_REQUEST_H
#define AWSMOCK_DTO_KMS_GET_KEY_COUNTER_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Get KMS key request
     *
     * Example:
     * @code{.json}
     * {
     *   "keyId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetKeyCounterRequest final : Common::BaseCounter<GetKeyCounterRequest> {

        /**
         * Key ID
         */
        std::string keyId;

      private:

        friend GetKeyCounterRequest tag_invoke(boost::json::value_to_tag<GetKeyCounterRequest>, boost::json::value const &v) {
            GetKeyCounterRequest r;
            r.keyId = Core::Json::GetStringValue(v, "keyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetKeyCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyId", obj.keyId},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_GET_KEY_COUNTER_REQUEST_H
