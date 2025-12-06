//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEYS_REQUEST_H
#define AWSMOCK_DTO_KMS_LIST_KEYS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief List all KMS keys request
     *
     * Example:
     * @code{.json}
     * {
     *   "Limit": number,
     *   "Marker": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKeysRequest final : Common::BaseCounter<ListKeysRequest> {

        /**
         * Limit
         */
        long limit{};

        /**
         * Marker for paging
         */
        std::string marker;

      private:

        friend ListKeysRequest tag_invoke(boost::json::value_to_tag<ListKeysRequest>, boost::json::value const &v) {
            ListKeysRequest r;
            r.limit = Core::Json::GetLongValue(v, "Limit");
            r.marker = Core::Json::GetStringValue(v, "Marker");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListKeysRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Limit", obj.limit},
                    {"Marker", boost::json::value_from(obj.marker)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_LIST_KEYS_REQUEST_H
