//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_USER_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_LIST_USER_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/transfer/model/User.h>

namespace AwsMock::Dto::Transfer {

    struct ListUserCountersResponse final : Common::BaseCounter<ListUserCountersResponse> {

        /**
         * List of attribute counters
         */
        std::vector<User> userCounters;

        /**
         * Total number of users
         */
        long total = 0;

      private:

        friend ListUserCountersResponse tag_invoke(boost::json::value_to_tag<ListUserCountersResponse>, boost::json::value const &v) {
            ListUserCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.userCounters = boost::json::value_to<std::vector<User>>(v.at("userCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"userCounters", boost::json::value_from(obj.userCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_USER_COUNTERS_RESPONSE_H
