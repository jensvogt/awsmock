//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERS_IN_GROUP_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERS_IN_GROUP_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/User.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users in a group response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersInGroupResponse final : Common::BaseDto<ListUsersInGroupResponse> {

        /**
         * User pool entities
         */
        std::vector<User> users;

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERS_IN_GROUP_RESPONSE_H
