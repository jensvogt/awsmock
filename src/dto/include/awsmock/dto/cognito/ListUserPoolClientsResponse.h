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
#include <awsmock/dto/common/BaseRequest.h>

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
    struct ListUserPoolClientsResponse : Common::BaseRequest {

        /**
         * User pool client entities
         */
        std::vector<UserPoolClient> userPoolsClients;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Convert to a JSON string.
         *
         * @return user pools json string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListUserPoolClientsResponse &i);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_RESPONSE_H
