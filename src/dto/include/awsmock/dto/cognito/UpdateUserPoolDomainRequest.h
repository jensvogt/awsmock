//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_REQUEST_H
#define AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/CustomDomainConfig.h>
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
     *   "CustomDomainConfig": {
     *      "CertificateArn": "string"
     *   },
     *   "Domain": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateUserPoolDomainRequest final : Common::BaseCounter<UpdateUserPoolDomainRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Domain
         */
        std::string domain;

        /**
         * Custom domain config
         */
        CustomDomainConfig customDomainConfig;

      private:

        friend UpdateUserPoolDomainRequest tag_invoke(boost::json::value_to_tag<UpdateUserPoolDomainRequest>, boost::json::value const &v) {
            UpdateUserPoolDomainRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.domain = Core::Json::GetStringValue(v, "Domain");
            r.customDomainConfig = boost::json::value_to<CustomDomainConfig>(v.at("CustomDomainConfig"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateUserPoolDomainRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"Domain", obj.domain},
                    {"CustomDomainConfig", boost::json::value_from(obj.customDomainConfig)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_REQUEST_H
