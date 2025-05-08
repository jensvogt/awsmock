//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_SERVER_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_LIST_SERVER_COUNTERS_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/transfer/model/Server.h>

namespace AwsMock::Dto::Transfer {

    struct ListServerCountersResponse final : Common::BaseCounter<ListServerCountersResponse> {

        /**
         * List of buckets
         */
        std::vector<Server> transferServers;

        /**
         * Total number of buckets
         */
        long total = 0;

      private:

        friend ListServerCountersResponse tag_invoke(boost::json::value_to_tag<ListServerCountersResponse>, boost::json::value const &v) {
            ListServerCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.transferServers = boost::json::value_to<std::vector<Server>>(v.at("protocolCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListServerCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"transferServers", boost::json::value_from(obj.transferServers)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_SERVER_COUNTERS_RESPONSE_H
