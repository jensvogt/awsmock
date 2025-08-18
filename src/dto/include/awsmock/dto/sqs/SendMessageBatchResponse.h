//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_RESPONSE_H
#define AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/common/BaseCounter.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/sqs/model/MessageFailed.h>
#include <awsmock/dto/sqs/model/MessageSuccessful.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Send message batch response
     *
     * Example:
     * @code{.json}
     * {
     *   "Failed": [
     *      {
     *         "Code": "string",
     *         "Id": "string",
     *         "Message": "string",
     *         "SenderFault": boolean
     *      }
     *   ],
     *   "Successful": [
     *      {
     *         "Id": "string",
     *         "MD5OfMessageAttributes": "string",
     *         "MD5OfMessageBody": "string",
     *         "MD5OfMessageSystemAttributes": "string",
     *         "MessageId": "string",
     *         "SequenceNumber": "string"
     *      }
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     *
     */
    struct SendMessageBatchResponse final : Common::BaseCounter<SendMessageBatchResponse> {

        /**
         * Successful messages
         */
        std::vector<MessageSuccessful> successful;

        /**
         * Failed messages
         */
        std::vector<MessageFailed> failed;

      private:

        friend SendMessageBatchResponse tag_invoke(boost::json::value_to_tag<SendMessageBatchResponse>, boost::json::value const &v) {
            SendMessageBatchResponse r;
            if (Core::Json::AttributeExists(v, "Successful")) {
                r.successful = boost::json::value_to<std::vector<MessageSuccessful>>(v.at("Successful"));
            }
            if (Core::Json::AttributeExists(v, "Failed")) {
                r.failed = boost::json::value_to<std::vector<MessageFailed>>(v.at("Failed"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SendMessageBatchResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
            };
            if (!obj.successful.empty()) {
                jv.as_object()["Successful"] = boost::json::value_from(obj.successful);
            }
            if (!obj.failed.empty()) {
                jv.as_object()["Failed"] = boost::json::value_from(obj.failed);
            }
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_RESPONSE_H
