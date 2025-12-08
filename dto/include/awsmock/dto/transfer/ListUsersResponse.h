//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_USER_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_LIST_USER_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/transfer/model/User.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief List FTP user response
     *
     * Example
     * @code{.json}
     * {
     *   "NextToken": "eyJNYXJrZXIiOiBudWxsLCAiYm90b1X0cnVuU2F0ZV9hbW91bnQiOiAyfQ==",
     *   "ServerId": "s-01234567890abcdef",
     *   "Users": [
     *      {
     *         "Arn": "arn:aws:transfer:us-east-1:176354371281:user/s-01234567890abcdef/charlie",
     *         "HomeDirectory": "/data/home/charlie",
     *         "SshPublicKeyCount": 1,
     *         "Role": "arn:aws:iam::176354371281:role/transfer-role1",
     *         "Tags": [
     *            {
     *               "Key": "Name",
     *               "Value": "user1"
     *            }
     *         ],
     *         "UserName": "my_user"
     *      }
     *   ]
     *   }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersResponse final : Common::BaseCounter<ListUsersResponse> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * Token
         */
        std::string nextToken;

        /**
         * Password
         */
        std::string password;

        /**
         * Next token ID
         */
        std::vector<User> users;

      private:

        friend ListUsersResponse tag_invoke(boost::json::value_to_tag<ListUsersResponse>, boost::json::value const &v) {
            ListUsersResponse r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.password = Core::Json::GetStringValue(v, "Password");
            r.users = boost::json::value_to<std::vector<User>>(v.at("Users"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUsersResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
                    {"NextToken", obj.nextToken},
                    {"Password", obj.password},
                    {"Users", boost::json::value_from(obj.users)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_USER_RESPONSE_H
