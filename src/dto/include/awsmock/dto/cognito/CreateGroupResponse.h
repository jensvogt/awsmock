//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CREATE_GROUP_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_CREATE_GROUP_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/Group.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

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
    struct CreateGroupResponse final : Common::BaseDto<CreateGroupResponse> {

        /**
         * Group
         */
        Group group;

        /**
         * @brief Convert to a JSON string.
         *
         * @return json string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CREATE_GROUP_RESPONSE_H
