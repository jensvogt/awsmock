//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_MESSAGE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_MESSAGE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/sns/model/MessageCounter.h>

namespace AwsMock::Dto::SNS {

    struct ListMessageCountersResponse final : Common::BaseCounter<ListMessageCountersResponse> {

        /**
         * List of queues messages
         */
        std::vector<MessageCounter> messages;

        /**
         * Total number of messages
         */
        long total = 0;

    private:

        friend ListMessageCountersResponse tag_invoke(boost::json::value_to_tag<ListMessageCountersResponse>, boost::json::value const &v) {
            ListMessageCountersResponse r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.total = static_cast<long>(v.at("total").as_int64());
            r.messages = boost::json::value_to<std::vector<MessageCounter> >(v.at("messages"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListMessageCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"messages", boost::json::value_from(obj.messages)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_MESSAGE_COUNTERS_RESPONSE_H
