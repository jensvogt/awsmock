//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_MESSAGES_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_MESSAGES_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/MessageEntry.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS messages response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListMessagesResponse final : Common::BaseCounter<ListMessagesResponse> {

        /**
         * List of queues messages
         */
        std::vector<MessageEntry> messages;

        /**
         * Total number of messages
         */
        long total = 0;

      private:

        friend ListMessagesResponse tag_invoke(boost::json::value_to_tag<ListMessagesResponse>, boost::json::value const &v) {
            ListMessagesResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "messages")) {
                r.messages = boost::json::value_to<std::vector<MessageEntry>>(v.at("messages"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListMessagesResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_MESSAGES_RESPONSE_H
