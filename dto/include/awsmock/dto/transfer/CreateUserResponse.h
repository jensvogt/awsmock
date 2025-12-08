//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_CREATE_USER_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_CREATE_USER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct CreateUserResponse final : Common::BaseCounter<CreateUserResponse> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * User name
         */
        std::string userName;

      private:

        friend CreateUserResponse tag_invoke(boost::json::value_to_tag<CreateUserResponse>, boost::json::value const &v) {
            CreateUserResponse r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.userName = Core::Json::GetBoolValue(v, "UserName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUserResponse const &obj) {
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

#endif// AWSMOCK_DTO_TRANSFER_CREATE_USER_RESPONSE_H
