//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_REQUEST_H
#define AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>
#include <awsmock/dto/sqs/model/MessageEntry.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Send message batch request
     *
     * Example (from Java v2 client):
     * @code{.json}
     * {
     *   "Entries": [
     *      {
     *         "DelaySeconds": number,
     *         "Id": "string",
     *         "MessageAttributes": {
     *            "string" : {
     *               "BinaryListValues": [ blob ],
     *               "BinaryValue": blob,
     *               "DataType": "string",
     *               "StringListValues": [ "string" ],
     *               "StringValue": "string"
     *            }
     *         },
     *         "MessageBody": "string",
     *         "MessageDeduplicationId": "string",
     *         "MessageGroupId": "string",
     *         "MessageSystemAttributes": {
     *            "string" : {
     *               "BinaryListValues": [ blob ],
     *               "BinaryValue": blob,
     *               "DataType": "string",
     *               "StringListValues": [ "string" ],
     *               "StringValue": "string"
     *            }
     *         }
     *      }
     *   ],
     *   "QueueUrl": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SendMessageBatchRequest final : Common::BaseCounter<SendMessageBatchRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue name
         */
        std::vector<MessageEntry> entries;

      private:

        friend SendMessageBatchRequest tag_invoke(boost::json::value_to_tag<SendMessageBatchRequest>, boost::json::value const &v) {
            SendMessageBatchRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            if (Core::Json::AttributeExists(v, "Entries")) {
                r.entries = boost::json::value_to<std::vector<MessageEntry>>(v.at("Entries"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SendMessageBatchRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
                    {"Entries", boost::json::value_from(obj.entries)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_REQUEST_H
