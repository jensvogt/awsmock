//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_ADD_USER_TO_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_ADD_USER_TO_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Adds a user to a group
     *
     * Example:
     * @code{.json}
     * {
     *   "GroupName": "string",
     *   "Username": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminAddUserToGroupRequest : Common::BaseRequest<AdminAddUserToGroupRequest> {

        /**
         * Name of the group
         */
        std::string groupName;

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * User pool id
         */
        std::string userPoolId;

        /**
         * @brief Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_ADD_USER_TO_GROUP_REQUEST_H
