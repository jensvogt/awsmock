//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/entity/cognito/User.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersResponse final : Common::BaseDto<ListUsersResponse> {

        /**
         * User entities
         */
        Database::Entity::Cognito::UserList users;

        /**
         * User entities
         */
        long total;

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERS_RESPONSE_H
