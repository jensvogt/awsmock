//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/sns/model/MessageAttribute.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

    struct PublishBatchRequest final : Common::BaseCounter<PublishBatchRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Target ARN
         */
        std::string targetArn;

        /**
         * Message body
         */
        std::string message;

        /**
         * MessageAttributes
         */
        std::map<std::string, MessageAttribute> messageAttributes;

    private:
        friend PublishBatchRequest tag_invoke(boost::json::value_to_tag<PublishBatchRequest>, boost::json::value const &v) {
            PublishBatchRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "TopicArn");
            r.targetArn = Core::Json::GetStringValue(v, "TargetArn");
            r.message = Core::Json::GetStringValue(v, "Message");
            if (Core::Json::AttributeExists(v, "MessageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute> >(v.at("MessageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PublishBatchRequest const &obj) {
            jv = {
                {"Region", obj.region},
                {"User", obj.user},
                {"RequestId", obj.requestId},
                {"TopicArn", obj.topicArn},
                {"TargetArn", obj.targetArn},
                {"Message", obj.message},
                {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };

} // namespace Awsmock::Dto::SNS
