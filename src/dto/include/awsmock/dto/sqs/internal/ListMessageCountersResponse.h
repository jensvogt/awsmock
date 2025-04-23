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
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.total = v.at("total").as_int64();

            // Queue counters
            if (v.as_object().contains("messages")) {
                for (const auto &o: v.at("messages").as_array()) {
                    r.messages.emplace_back(boost::json::value_to<MessageCounter>(o));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListMessageCountersResponse const &obj) {

            // Message counters
            boost::json::array messageCounters;
            if (!obj.messages.empty()) {
                for (const auto &o: obj.messages) {
                    messageCounters.emplace_back(boost::json::value_from(o));
                }
            }

            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"messageCounters", messageCounters},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_MESSAGE_COUNTERS_RESPONSE_H
