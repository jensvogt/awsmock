//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEY_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_KMS_LIST_KEY_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/Key.h>
#include <awsmock/dto/kms/model/KeyCounter.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief List all KMS keys request
     *
     * Example:
     * @code{.json}
     * {
     *   "keys": [
     *     {
     *       "keyArn": "string",
     *       "keyId": "string"
     *     }
     *   ],
     *  "total": "long",
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKeyCountersResponse final : Common::BaseCounter<ListKeyCountersResponse> {

        /**
         * Key counters
         */
        std::vector<KeyCounter> keyCounters;

        /**
         * Total number of keys
         */
        long total{};

      private:

        friend ListKeyCountersResponse tag_invoke(boost::json::value_to_tag<ListKeyCountersResponse>, boost::json::value const &v) {
            ListKeyCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.keyCounters = boost::json::value_to<std::vector<KeyCounter>>(v.at("keyCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListKeyCountersResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"total", obj.total},
                    {"keyCounters", boost::json::value_from(obj.keyCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_LIST_KEY_COUNTERS_RESPONSE_H
