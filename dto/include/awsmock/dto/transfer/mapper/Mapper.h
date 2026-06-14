//
// Created by vogje01 on 6/2/24.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/transfer/ListUsersRequest.h>
#include <awsmock/dto/transfer/ListUsersResponse.h>
#include <awsmock/dto/transfer/model/User.h>
#include <awsmock/entity/transfer/User.h>

namespace Awsmock::Dto::Transfer {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a transfer user list to a list users response
         *
         * Some values will be pulled over from the request.
         *
         * @param userList transfer user entity list
         * @param request request struct
         * @return ListUsersResponse
         * @see ListUsersResponse
         */
        static ListUsersResponse map(const ListUsersRequest &request, const std::vector<Database::Entity::Transfer::User> &userList);
    };

}// namespace Awsmock::Dto::Transfer
