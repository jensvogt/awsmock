//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Updates an existing user pool domain request.
     *
     * Request to update an existing user pool domain.
     *
     * Example:
     * @code{.json}
     * {
     *   "CloudFrontDomain": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateUserPoolDomainResponse final : Common::BaseCounter<UpdateUserPoolDomainResponse> {

        /**
         * User pool ID
         */
        std::string cloudFrontDomain;

      private:

        friend UpdateUserPoolDomainResponse tag_invoke(boost::json::value_to_tag<UpdateUserPoolDomainResponse>, boost::json::value const &v) {
            UpdateUserPoolDomainResponse r;
            r.cloudFrontDomain = Core::Json::GetStringValue(v, "CloudFrontDomain");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateUserPoolDomainResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"CloudFrontDomain", obj.cloudFrontDomain},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_RESPONSE_H
