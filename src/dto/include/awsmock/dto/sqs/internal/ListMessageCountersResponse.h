//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_MESSAGE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_MESSAGE_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageCounter.h>

namespace AwsMock::Dto::SQS {

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
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "messages")) {
                r.messages = boost::json::value_to<std::vector<MessageCounter>>(v.at("messages"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListMessageCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"messageCounters", boost::json::value_from(obj.messages)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_MESSAGE_COUNTERS_RESPONSE_H
