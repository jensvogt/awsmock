//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_STOP_SERVER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_STOP_SERVER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct StopServerRequest final : Common::BaseCounter<StopServerRequest> {

        /**
         * Maximal number of results
         */
        std::string serverId;

      private:

        friend StopServerRequest tag_invoke(boost::json::value_to_tag<StopServerRequest>, boost::json::value const &v) {
            StopServerRequest r;
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StopServerRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ServerId", obj.serverId},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_STOP_SERVER_REQUEST_H
