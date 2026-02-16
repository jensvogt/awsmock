//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_REDRIVE_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_REDRIVE_MESSAGE_REQUEST_H

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Redrive messages request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RedriveMessageRequest final : Common::BaseCounter<RedriveMessageRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Message ID
         */
        std::string messageId;

      private:

        friend RedriveMessageRequest tag_invoke(boost::json::value_to_tag<RedriveMessageRequest>, boost::json::value const &v) {
            RedriveMessageRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "QueueArn");
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RedriveMessageRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueArn", obj.queueArn},
                    {"MessageId", obj.messageId},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_REDRIVE_MESSAGES_REQUEST_H
