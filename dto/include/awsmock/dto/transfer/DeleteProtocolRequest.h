//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_DELETE_PROTOCOL_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_DELETE_PROTOCOL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/transfer/model/Protocol.h>

namespace AwsMock::Dto::Transfer {

    struct DeleteProtocolRequest final : Common::BaseCounter<DeleteProtocolRequest> {

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * Protocol
         */
        ProtocolType protocol;

      private:

        friend DeleteProtocolRequest tag_invoke(boost::json::value_to_tag<DeleteProtocolRequest>, boost::json::value const &v) {
            DeleteProtocolRequest r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.protocol = ProtocolTypeFromString(Core::Json::GetStringValue(v, "Protocol"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteProtocolRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
                    {"Protocol", ProtocolTypeToString(obj.protocol)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_DELETE_PROTOCOL_REQUEST_H
