//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEYS_RESPONSE_H
#define AWSMOCK_DTO_KMS_LIST_KEYS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/Key.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief List all KMS keys request
     *
     * Example:
     * @code{.json}
     * {
     *   "Keys": [
     *     {
     *       "KeyArn": "string",
     *       "KeyId": "string"
     *     }
     *   ],
     *  "NextMarker": "string",
     *  "Truncated": boolean
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKeysResponse final : Common::BaseCounter<ListKeysResponse> {

        /**
         * Key
         */
        std::vector<Key> keys;

        /**
         * Next marker for paging
         */
        std::string nextMarker;

        /**
         * Truncated
         */
        bool truncated = false;

      private:

        friend ListKeysResponse tag_invoke(boost::json::value_to_tag<ListKeysResponse>, boost::json::value const &v) {
            ListKeysResponse r;
            r.nextMarker = Core::Json::GetStringValue(v, "NextMarker");
            r.truncated = Core::Json::GetBoolValue(v, "Truncated");
            r.keys = boost::json::value_to<std::vector<Key>>(v.at("Keys"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListKeysResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"NextMarker", obj.nextMarker},
                    {"Truncated", obj.truncated},
                    {"Keys", boost::json::value_from(obj.keys)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_LIST_KEYS_RESPONSE_H
