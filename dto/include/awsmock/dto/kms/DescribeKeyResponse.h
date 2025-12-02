//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_DESCRIBE_KEY_RESPONSE_H
#define AWSMOCK_DTO_KMS_DESCRIBE_KEY_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/kms/model/Key.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Create KMS key request
     *
     * Example:
     * @code{.json}
     * {
     *   "GrantTokens": [ "string" ],
     *   "KeyId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeKeyResponse final : Common::BaseCounter<DescribeKeyResponse> {

        /**
         * Key metadata
         */
        Key key;

      private:

        friend DescribeKeyResponse tag_invoke(boost::json::value_to_tag<DescribeKeyResponse>, boost::json::value const &v) {
            DescribeKeyResponse r;
            r.key = boost::json::value_to<Key>(v.at("Key"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeKeyResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Key", boost::json::value_from(obj.key)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_DESCRIBE_KEY_RESPONSE_H
