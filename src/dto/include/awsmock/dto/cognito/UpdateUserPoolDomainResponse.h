//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

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
    struct UpdateUserPoolDomainResponse : Common::BaseDto<UpdateUserPoolDomainResponse> {

        /**
         * User pool ID
         */
        std::string cloudFrontDomain;

        /**
         * @brief Convert from a JSON object.
         *
         * @param jsonString json string object
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_DOMAIN_RESPONSE_H
