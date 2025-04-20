//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool clients request
     *
     * @code(.json)
     * {
     *   "MaxResults": number,
     *   "NextToken": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolClientsRequest final : Common::BaseRequest<ListUserPoolClientsRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId{};

        /**
         * Max results
         */
        int maxResults;

        /**
         * Next token
         */
        std::string nextToken;

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
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_REQUEST_H
