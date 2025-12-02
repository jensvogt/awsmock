//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_CREATE_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_CREATE_QUEUE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/QueueAttribute.h>

namespace AwsMock::Dto::SQS {

    struct CreateQueueRequest final : Common::BaseCounter<CreateQueueRequest> {

        /**
         * Queue name
         */
        std::string queueName;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Attributes
         */
        std::map<std::string, std::string> attributes;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend CreateQueueRequest tag_invoke(boost::json::value_to_tag<CreateQueueRequest>, boost::json::value const &v) {
            CreateQueueRequest r;
            r.queueName = Core::Json::GetStringValue(v, "QueueName");
            r.queueUrl = Core::CreateSQSQueueUrl(Core::Json::GetStringValue(v, "QueueName"));
            r.queueArn = Core::CreateSQSQueueArn(Core::Json::GetStringValue(v, "QueueName"));
            r.owner = Core::Json::GetStringValue(v, "Owner");
            if (Core::Json::AttributeExists(v, "Attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("Attributes"));
            }
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateQueueRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueName", obj.queueName},
                    {"QueueUrl", obj.queueUrl},
                    {"QueueArn", obj.queueArn},
                    {"Attributes", boost::json::value_from(obj.attributes)},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_CREATE_QUEUE_REQUEST_H
