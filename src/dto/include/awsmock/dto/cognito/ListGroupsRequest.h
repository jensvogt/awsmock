//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_GROUPS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_GROUPS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List groups request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListGroupsRequest final : Common::BaseCounter<ListGroupsRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Limit
         */
        long limit;

        /**
         * Next token
         */
        std::string nextToken;

      private:

        friend ListGroupsRequest tag_invoke(boost::json::value_to_tag<ListGroupsRequest>, boost::json::value const &v) {
            ListGroupsRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.limit = Core::Json::GetLongValue(v, "Limit");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListGroupsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"Limit", obj.limit},
                    {"NextToken", obj.nextToken},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_GROUPS_REQUEST_H
