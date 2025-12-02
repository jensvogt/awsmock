//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_GET_MESSAGE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_GET_MESSAGE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/sns/model/Message.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/MessageAttribute.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief Get a message request
     *
     * Example (from Java v2 client):
     * @code{.json}
     * {
     *   "messageId": "string",
     * }
     * @endcode
     */
    struct GetMessageCountersResponse final : Common::BaseCounter<GetMessageCountersResponse> {

        /**
         * Message
         */
        Message message;

    private:

        friend GetMessageCountersResponse tag_invoke(boost::json::value_to_tag<GetMessageCountersResponse>, boost::json::value const &v) {
            GetMessageCountersResponse r;
            r.message = boost::json::value_to<Message>(v.at("message"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetMessageCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"message", boost::json::value_from(obj.message)},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SNS_GET_MESSAGE_COUNTERS_RESPONSE_H
