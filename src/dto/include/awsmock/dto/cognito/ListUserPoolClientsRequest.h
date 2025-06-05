//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

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
    struct ListUserPoolClientsRequest final : Common::BaseCounter<ListUserPoolClientsRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId{};

        /**
         * Max results
         */
        int maxResults{};

        /**
         * Next token
         */
        std::string nextToken;

      private:

        friend ListUserPoolClientsRequest tag_invoke(boost::json::value_to_tag<ListUserPoolClientsRequest>, boost::json::value const &v) {
            ListUserPoolClientsRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.maxResults = Core::Json::GetLongValue(v, "MaxResults");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserPoolClientsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"MaxResults", obj.maxResults},
                    {"NextToken", obj.nextToken},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_CLIENTS_REQUEST_H
