//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEY_ARNS_RESPONSE_H
#define AWSMOCK_DTO_KMS_LIST_KEY_ARNS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/Key.h>
#include <awsmock/dto/kms/model/KeyCounter.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief List all KMS key ARNs request
     *
     * Example:
     * @code{.json}
     * {
     *   "keyArns": [<string>...]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKeyArnsResponse final : Common::BaseCounter<ListKeyArnsResponse> {

        /**
         * Key counters
         */
        std::vector<std::string> keyArns;

      private:

        friend ListKeyArnsResponse tag_invoke(boost::json::value_to_tag<ListKeyArnsResponse>, boost::json::value const &v) {
            ListKeyArnsResponse r;
            if (Core::Json::AttributeExists(v, "keyArns")) {
                r.keyArns = boost::json::value_to<std::vector<std::string>>(v.at("keyArns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListKeyArnsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyArns", boost::json::value_from(obj.keyArns)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_LIST_KEY_ARNS_RESPONSE_H
