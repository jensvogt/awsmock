//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_DELETE_USER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_DELETE_USER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct DeleteUserRequest final : Common::BaseCounter<DeleteUserRequest> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * User name
         */
        std::string userName;

      private:

        friend DeleteUserRequest tag_invoke(boost::json::value_to_tag<DeleteUserRequest>, boost::json::value const &v) {
            DeleteUserRequest r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.userName = Core::Json::GetStringValue(v, "UserName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteUserRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
                    {"UserName", obj.userName},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_DELETE_USER_REQUEST_H
