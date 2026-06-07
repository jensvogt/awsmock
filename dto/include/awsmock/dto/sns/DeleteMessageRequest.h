//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

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

}// namespace Awsmock::Dto::SNS
