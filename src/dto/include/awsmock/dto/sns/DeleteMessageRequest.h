//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_DELETE_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SNS_DELETE_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief Delete message request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteMessageRequest final : Common::BaseCounter<DeleteMessageRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Message ID
         */
        std::string messageId;

      private:

        friend DeleteMessageRequest tag_invoke(boost::json::value_to_tag<DeleteMessageRequest>, boost::json::value const &v) {
            DeleteMessageRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageRequest const &obj) {
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

#endif// AWSMOCK_DTO_SNS_DELETE_MESSAGE_REQUEST_H
