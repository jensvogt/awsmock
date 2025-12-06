//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_MESSAGES_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_MESSAGES_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/Message.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief List messages response
     *
     * @par
     * This is not an AWS SDK response, but only available in awsmock
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListMessagesResponse final : Common::BaseCounter<ListMessagesResponse> {

        /**
         * List of messages
         */
        std::vector<Message> messages;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Total number of messages
         */
        long total;

      private:

        friend ListMessagesResponse tag_invoke(boost::json::value_to_tag<ListMessagesResponse>, boost::json::value const &v) {
            ListMessagesResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "messages")) {
                r.messages = boost::json::value_to<std::vector<Message>>(v.at("messages"));
            }

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListMessagesResponse const &obj) {
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

#endif// AWSMOCK_DTO_SQS_LIST_TOPICS_RESPONSE_H
