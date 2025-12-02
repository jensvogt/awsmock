//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_CREATE_PROTOCOL_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_CREATE_PROTOCOL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/transfer/model/Protocol.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Creates a new protocol request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateProtocolRequest final : Common::BaseCounter<CreateProtocolRequest> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * Protocol
         */
        ProtocolType protocol = ProtocolType::UNKNOWN;

        /**
         * Port
         */
        long port{};

      private:

        friend CreateProtocolRequest tag_invoke(boost::json::value_to_tag<CreateProtocolRequest>, boost::json::value const &v) {
            CreateProtocolRequest r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.protocol = ProtocolTypeFromString(Core::Json::GetStringValue(v, "Protocol"));
            r.port = Core::Json::GetLongValue(v, "Port");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateProtocolRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
                    {"Protocol", ProtocolTypeToString(obj.protocol)},
                    {"Port", obj.port},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_CREATE_PROTOCOL_REQUEST_H
