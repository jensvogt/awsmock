//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct CreateUserRequest final : Common::BaseCounter<CreateUserRequest> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * User name
         */
        std::string userName;

        /**
         * Password
         */
        std::string password;

        /**
         * Home directory
         */
        std::string homeDirectory;

      private:

        friend CreateUserRequest tag_invoke(boost::json::value_to_tag<CreateUserRequest>, boost::json::value const &v) {
            CreateUserRequest r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.userName = Core::Json::GetBoolValue(v, "UserName");
            r.password = Core::Json::GetBoolValue(v, "Password");
            r.homeDirectory = Core::Json::GetBoolValue(v, "HomeDirectory");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUserRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
                    {"UserName", obj.userName},
                    {"Password", obj.password},
                    {"HomeDirectory", obj.homeDirectory},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_H
