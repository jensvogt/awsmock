//
// Created by vogje01 on 6/2/24.
//

#ifndef AWSMOCK_DTO_TRANSFER_MAPPER_H
#define AWSMOCK_DTO_TRANSFER_MAPPER_H

// AwsMock includes
#include <awsmock/dto/transfer/ListUsersRequest.h>
#include <awsmock/dto/transfer/ListUsersResponse.h>
#include <awsmock/dto/transfer/model/User.h>
#include <awsmock/entity/transfer/User.h>

namespace AwsMock::Dto::Transfer {

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

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_MAPPER_H
