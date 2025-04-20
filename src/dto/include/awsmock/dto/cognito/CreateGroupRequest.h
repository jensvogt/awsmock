//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CREATE_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_CREATE_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create group request
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
    struct CreateGroupRequest final : Common::BaseRequest<CreateGroupRequest> {

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
        int precedence;

        /**
         * Role ARN
         */
        std::string roleArn;

        /**
         * UserPoolId
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

#endif// AWSMOCK_DTO_COGNITO_CREATE_GROUP_REQUEST_H
