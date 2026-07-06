//
// Created by vogje01 on 11/25/23.
//

#pragma once
// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/Group.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Cognito {

    /**
     * @brief Create group response
     *
     * Example:
     * @code{.json}
     * {
     *   "Group": {
     *      "CreationDate": number,
     *      "Description": "string",
     *      "GroupName": "string",
     *      "LastModifiedDate": number,
     *      "Precedence": number,
     *      "RoleArn": "string",
     *      "UserPoolId": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateGroupResponse final : Common::BaseCounter<CreateGroupResponse> {

        /**
         * Group
         */
        Group group;

      private:

        friend CreateGroupResponse tag_invoke(boost::json::value_to_tag<CreateGroupResponse>, boost::json::value const &v) {
            CreateGroupResponse r;
            r.group = boost::json::value_to<Group>(v, "group");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateGroupResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Group", boost::json::value_from(obj.group)},
            };
        }
    };

}// namespace Awsmock::Dto::Cognito

