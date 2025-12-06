//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_RESPONSE_H
#define AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "model/Message.h"


#include <awsmock/dto/sqs/model/MessageAttribute.h>
#include <awsmock/entity/sqs/Message.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Receive message response.
     *
     * Example:
     * @code{.json}
     * {
     *   "Messages": [
     *      {
     *         "Attributes": {
     *            "string" : "string"
     *         },
     *         "Body": "string",
     *         "MD5OfBody": "string",
     *         "MD5OfMessageAttributes": "string",
     *         "MessageAttributes": {
     *            "string" : {
     *               "BinaryListValues": [ blob ],
     *               "BinaryValue": blob,
     *               "DataType": "string",
     *               "StringListValues": [ "string" ],
     *               "StringValue": "string"
     *            }
     *         },
     *         "MessageId": "string",
     *         "ReceiptHandle": "string"
     *      }
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ReceiveMessageResponse final : Common::BaseCounter<ReceiveMessageResponse> {

        /**
         * Message list
         */
        std::vector<Message> messageList;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const;

      private:

        friend ReceiveMessageResponse tag_invoke(boost::json::value_to_tag<ReceiveMessageResponse>, boost::json::value const &v) {
            ReceiveMessageResponse r;
            r.messageList = boost::json::value_to<std::vector<Message>>(v.at("messageList"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ReceiveMessageResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Messages", boost::json::value_from(obj.messageList)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_RESPONSE_H
