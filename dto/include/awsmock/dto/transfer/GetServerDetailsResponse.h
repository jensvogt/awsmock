//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/transfer/model/Server.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Get transfer server details response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetServerDetailsResponse final : Common::BaseCounter<GetServerDetailsResponse> {

        /**
         * Region
         */
        Server server;

      private:

        friend GetServerDetailsResponse tag_invoke(boost::json::value_to_tag<GetServerDetailsResponse>, boost::json::value const &v) {
            GetServerDetailsResponse r;
            r.server = boost::json::value_to<Server>(v.at("ServerId"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetServerDetailsResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Server", boost::json::value_from(obj.server)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_RESPONSE_H
