//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/UserPoolClient.h>
#include <awsmock/dto/common/BaseCounter.h>

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
    struct ListUserPoolClientsResponse final : Common::BaseCounter<ListUserPoolClientsResponse> {

        /**
         * User pool client entities
         */
        std::vector<UserPoolClient> userPoolsClients;

        /**
         * Next token
         */
        std::string nextToken;

      private:

        friend ListUserPoolClientsResponse tag_invoke(boost::json::value_to_tag<ListUserPoolClientsResponse>, boost::json::value const &v) {
            ListUserPoolClientsResponse r;
            r.userPoolsClients = boost::json::value_to<std::vector<UserPoolClient>>(v.at("UserPoolsClients"));
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserPoolClientsResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolsClients", boost::json::value_from(obj.userPoolsClients)},
                    {"NextToken", obj.nextToken},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_RESPONSE_H
