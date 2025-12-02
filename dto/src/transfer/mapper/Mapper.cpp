//
// Created by vogje01 on 6/2/24.
//

#include <awsmock/dto/transfer/mapper/Mapper.h>

namespace AwsMock::Dto::Transfer {

    ListUsersResponse Mapper::map(const ListUsersRequest &request, const std::vector<Database::Entity::Transfer::User> &userList) {

        ListUsersResponse response;
        response.serverId = request.serverId;

        for (const auto &u: userList) {
            User user;
            user.userName = u.userName;
            user.arn = u.arn;
            user.homeDirectory = u.homeDirectory;
            user.password = u.password;
            response.users.emplace_back(user);
        }
        return response;
    }

}// namespace AwsMock::Dto::Transfer
