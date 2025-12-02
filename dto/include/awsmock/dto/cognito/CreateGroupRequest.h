//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CREATE_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_CREATE_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create a group request
     *
     * @par
     * Request to create a new group.
     *
     * Example:
     * @code{.json}
     * {
     *   "Description": "string",
     *   "GroupName": "string",
     *   "Precedence": number,
     *   "RoleArn": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateGroupRequest final : Common::BaseCounter<CreateGroupRequest> {

        /**
         * UserPoolId
         */
        std::string userPoolId;

        /**
         * Name of the group
         */
        std::string groupName;

        /**
         * Description
         */
        std::string description;

        /**
         * Precedence
         */
        long precedence{};

        /**
         * Role ARN
         */
        std::string roleArn;

      private:

        friend CreateGroupRequest tag_invoke(boost::json::value_to_tag<CreateGroupRequest>, boost::json::value const &v) {
            CreateGroupRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.groupName = Core::Json::GetStringValue(v, "groupName");
            r.description = Core::Json::GetStringValue(v, "description");
            r.precedence = Core::Json::GetLongValue(v, "precedence");
            r.roleArn = Core::Json::GetStringValue(v, "roleArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateGroupRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userPoolId", obj.userPoolId},
                    {"groupName", obj.groupName},
                    {"description", obj.description},
                    {"precedence", obj.precedence},
                    {"roleArn", obj.roleArn},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CREATE_GROUP_REQUEST_H
