//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_RESEND_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_RESEND_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Update message request
     *
     * Example (from Java v2 client):
     * @code{.json}
     * {
     *   "QueueUrl":"http://localhost:4566/000000000000/artikel-input",
     *   "ReceiptHandle":"string"
     * }
     * @endcode
     */
    struct ResendMessageRequest final : Common::BaseCounter<ResendMessageRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Message ID
         */
        std::string messageId;

      private:

        friend ResendMessageRequest tag_invoke(boost::json::value_to_tag<ResendMessageRequest>, boost::json::value const &v) {
            ResendMessageRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ResendMessageRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"messageId", obj.messageId},
            };
        }
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_RESEND_MESSAGE_REQUEST_H
