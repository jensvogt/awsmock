//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_CREATE_QUEUE_RESPONSE_H
#define AWSMOCK_DTO_SQS_CREATE_QUEUE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct CreateQueueResponse final : Common::BaseCounter<CreateQueueResponse> {

        /**
         * Name
         */
        std::string queueName;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue ARN
         */
        std::string queueArn;

      private:

        friend CreateQueueResponse tag_invoke(boost::json::value_to_tag<CreateQueueResponse>, boost::json::value const &v) {
            CreateQueueResponse r;
            r.queueName = Core::Json::GetStringValue(v, "QueueName");
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.queueArn = Core::Json::GetStringValue(v, "QueueArn");
            r.owner = Core::Json::GetStringValue(v, "Owner");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateQueueResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueName", obj.queueName},
                    {"QueueUrl", obj.queueUrl},
                    {"QueueArn", obj.queueArn},
                    {"Owner", obj.owner},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_CREATE_QUEUE_RESPONSE_H
