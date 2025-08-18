//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_DOMAIN_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_DOMAIN_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create user pool domain response
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
    struct CreateUserPoolDomainResponse final : Common::BaseCounter<CreateUserPoolDomainResponse> {

        /**
         * Name of the domain
         */
        std::string cloudFrontDomain;

      private:

        friend CreateUserPoolDomainResponse tag_invoke(boost::json::value_to_tag<CreateUserPoolDomainResponse>, boost::json::value const &v) {
            CreateUserPoolDomainResponse r;
            r.cloudFrontDomain = Core::Json::GetStringValue(v, "CloudFrontDomain");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUserPoolDomainResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"CloudFrontDomain", obj.cloudFrontDomain},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_DOMAIN_RESPONSE_H
