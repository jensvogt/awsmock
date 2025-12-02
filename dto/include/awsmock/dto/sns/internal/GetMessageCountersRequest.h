//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_GET_MESSAGE_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SNS_GET_MESSAGE_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

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
    struct GetMessageCountersRequest final : Common::BaseCounter<GetMessageCountersRequest> {

        /**
         * Message ID
         */
        std::string messageId;

    private:

        friend GetMessageCountersRequest tag_invoke(boost::json::value_to_tag<GetMessageCountersRequest>, boost::json::value const &v) {
            GetMessageCountersRequest r;
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetMessageCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"messageId", obj.messageId},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SNS_GET_MESSAGE_COUNTERS_REQUEST_H
