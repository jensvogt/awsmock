//
// Created by vogje01 on 11/25/23.
//

#pragma once
// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Cognito {

    /**
     * @brief Describe user pool domain request
     *
     * Request to describe a user pool domain.
     *
     * Example:
     * @code{.json}
     * {
     *   "Domain": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeUserPoolDomainRequest final : Common::BaseCounter<DescribeUserPoolDomainRequest> {

        /**
         * Domain name
         */
        std::string domain;

      private:

        friend DescribeUserPoolDomainRequest tag_invoke(boost::json::value_to_tag<DescribeUserPoolDomainRequest>, boost::json::value const &v) {
            DescribeUserPoolDomainRequest r;
            r.domain = Core::Json::GetStringValue(v, "Domain");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeUserPoolDomainRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Domain", obj.domain},
            };
        }
    };

}// namespace Awsmock::Dto::Cognito

