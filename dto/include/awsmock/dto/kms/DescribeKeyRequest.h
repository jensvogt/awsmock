//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_DESCRIBE_KEY_REQUEST_H
#define AWSMOCK_DTO_KMS_DESCRIBE_KEY_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

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
    struct DescribeKeyRequest final : Common::BaseCounter<DescribeKeyRequest> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Grant token
         */
        std::vector<std::string> grantTokens;

      private:

        friend DescribeKeyRequest tag_invoke(boost::json::value_to_tag<DescribeKeyRequest>, boost::json::value const &v) {
            DescribeKeyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            if (Core::Json::AttributeExists(v, "GrantTokens")) {
                r.grantTokens = boost::json::value_to<std::vector<std::string>>(v.at("GrantTokens"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeKeyRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"GrantTokens", boost::json::value_from(obj.grantTokens)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_DESCRIBE_KEY_REQUEST_H
