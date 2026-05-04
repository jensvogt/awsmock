//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_RESEND_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SNS_RESEND_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief Resend a single message in the topic
     *
     * @par
     * This is not an AWS SDK request, but only available in awsmock
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ResendMessageRequest final : Common::BaseCounter<ResendMessageRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Message ID
         */
        std::string messageId;

      private:

        friend ResendMessageRequest tag_invoke(boost::json::value_to_tag<ResendMessageRequest>, boost::json::value const &v) {
            ResendMessageRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ResendMessageRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"messageId", obj.messageId},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_RESEND_MESSAGE_REQUEST_H
