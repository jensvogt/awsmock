//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_CREATE_SERVER_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_CREATE_SERVER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/core/JsonUtils.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct CreateServerResponse final : Common::BaseCounter<CreateServerResponse> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * ARN of the transfer
         */
        std::string arn;

      private:

        friend CreateServerResponse tag_invoke(boost::json::value_to_tag<CreateServerResponse>, boost::json::value const &v) {
            CreateServerResponse r;
            r.serverId = Core::Json::GetStringValue(v, "Domain");
            r.arn = Core::Json::GetStringValue(v, "Arn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateServerResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
                    {"ServerId", obj.serverId},
                    {"Arn", obj.arn}};
        }
    };
}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_CREATE_SERVER_RESPONSE_H
