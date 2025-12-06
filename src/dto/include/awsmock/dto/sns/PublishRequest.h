//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_PUBLISH_REQUEST_H
#define AWSMOCK_DTO_SNS_PUBLISH_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/sns/model/MessageAttribute.h>

namespace AwsMock::Dto::SNS {

    struct PublishRequest final : Common::BaseCounter<PublishRequest> {

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

        friend PublishRequest tag_invoke(boost::json::value_to_tag<PublishRequest>, boost::json::value const &v) {
            PublishRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "TopicArn");
            r.targetArn = Core::Json::GetStringValue(v, "TargetArn");
            r.message = Core::Json::GetStringValue(v, "Message");
            if (Core::Json::AttributeExists(v, "MessageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("MessageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PublishRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"targetArn", obj.targetArn},
                    {"message", obj.message},
                    {"messageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_PUBLISH_REQUEST_H
