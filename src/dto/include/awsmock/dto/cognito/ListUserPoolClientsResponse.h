//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/UserPoolClient.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool clients response
     *
     * @code(.json)
     * {
     *   "NextToken": "string",
     *   "UserPoolClients": [
     *      {
     *         "ClientId": "string",
     *         "ClientName": "string",
     *         "UserPoolId": "string"
     *      }
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolClientsResponse final : Common::BaseDto<ListUserPoolClientsResponse> {

        /**
         * User pool client entities
         */
        std::vector<UserPoolClient> userPoolsClients;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_RESPONSE_H
