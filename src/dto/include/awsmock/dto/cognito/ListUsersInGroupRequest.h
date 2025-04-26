//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USER_IN_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USER_IN_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users in a group request
     *
     * Example:
     * @code{.json}
     * {
     *   "GroupName": "string",
     *   "Limit": number,
     *   "NextToken": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersInGroupRequest : Common::BaseDto<ListUsersInGroupRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Name of the group
         */
        std::string groupName;

        /**
         * Limit
         */
        int limit;

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
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USER_IN_GROUP_REQUEST_H
