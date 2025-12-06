//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DTO_SQS_QUEUE_H
#define AWSMOCK_DTO_SQS_QUEUE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>
#include <awsmock/dto/sqs/model/QueueAttribute.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    /**
     * SQS queue entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Queue final : Common::BaseCounter<Queue> {

        /**
         * Queue name
         */
        std::string name;

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

        /**
         * Queue userAttributes
         */
        QueueAttribute attributes;

        /**
         * Queue tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Queue size in bytes
         */
        long size = 0;

        /**
         * DLQ flag
         */
        bool isDlq = false;

        /**
         * Main queue for this DLQ
         */
        std::string mainQueue;

        /**
         * Default message attributes
         */
        std::map<std::string, MessageAttribute> defaultMessageAttributes;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

      private:

        friend Queue tag_invoke(boost::json::value_to_tag<Queue>, boost::json::value const &v) {
            Queue r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.owner = Core::Json::GetStringValue(v, "owner");
            r.queueUrl = Core::Json::GetStringValue(v, "queueUrl");
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.size = Core::Json::GetLongValue(v, "size");
            r.isDlq = Core::Json::GetBoolValue(v, "isDlq");
            r.mainQueue = Core::Json::GetStringValue(v, "mainQueue");
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "modified"));
            if (Core::Json::AttributeExists(v, "attributes")) {
                r.attributes = boost::json::value_to<Dto::SQS::QueueAttribute>(v.at("attributes"));
            }
            if (Core::Json::AttributeExists(v, "defaultMessageAttributes")) {
                r.defaultMessageAttributes = boost::json::value_to<std::map<std::string, Dto::SQS::MessageAttribute>>(v.at("defaultMessageAttributes"));
            }
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Queue const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"owner", obj.owner},
                    {"queueUrl", obj.queueUrl},
                    {"queueArn", obj.queueArn},
                    {"size", obj.size},
                    {"isDlq", obj.isDlq},
                    {"mainQueue", obj.mainQueue},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"defaultMessageAttributes", boost::json::value_from(obj.defaultMessageAttributes)},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

    typedef std::vector<Queue> QueueList;

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_QUEUE_H
