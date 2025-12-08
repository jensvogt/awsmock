//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct GetServerDetailsRequest final : Common::BaseCounter<GetServerDetailsRequest> {

        /**
         * Server ID
         */
        std::string serverId;

      private:

        friend GetServerDetailsRequest tag_invoke(boost::json::value_to_tag<GetServerDetailsRequest>, boost::json::value const &v) {
            GetServerDetailsRequest r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetServerDetailsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_REQUEST_H
