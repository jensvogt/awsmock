//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_COUNTER_H
#define AWSMOCK_DTO_SQS_MESSAGE_COUNTER_H

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    /**
     * @brief SQS message counter as used by the frontend
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MessageCounter final : Common::BaseCounter<MessageCounter> {

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
         * Message ID
         */
        std::string messageId;

        /**
         * ID
         */
        std::string id;

        /**
         * Receipt handle
         */
        std::string receiptHandle;

        /**
         * Body
         */
        std::string body;

        /**
         * System attributes
         */
        std::map<std::string, std::string> attributes;

        /**
         * Message attributes
         */
        std::map<std::string, MessageAttribute> messageAttributes;

        /**
         * MD5 sum
         */
        std::string md5Sum;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * Message size
         */
        long size = 0;

        /**
         * Message retries
         */
        long retries = 0;

        /**
         * Created time stamp
         */
        system_clock::time_point created = Core::DateTimeUtils::LocalDateTimeNow();

        /**
         * Created time stamp
         */
        system_clock::time_point modified;

      private:

        friend MessageCounter tag_invoke(boost::json::value_to_tag<MessageCounter>, boost::json::value const &v) {
            MessageCounter r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.queueName = v.at("queueName").as_string();
            r.queueUrl = v.at("queueUrl").as_string();
            r.queueArn = v.at("queueArn").as_string();
            r.messageId = v.at("messageId").as_string();
            r.id = v.at("id").as_string();
            r.receiptHandle = v.at("receiptHandle").as_string();
            r.body = v.at("body").as_string();
            r.md5Sum = v.at("md5Sum").as_string();
            r.size = v.at("size").as_int64();
            r.retries = v.at("retries").as_int64();
            r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("attributes"));
            r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("messageAttributes"));
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"contentType", obj.contentType},
                    {"queueName", obj.queueName},
                    {"queueUrl", obj.queueUrl},
                    {"queueArn", obj.queueArn},
                    {"messageId", obj.messageId},
                    {"id", obj.id},
                    {"receiptHandle", obj.receiptHandle},
                    {"body", obj.body},
                    {"md5Sum", obj.md5Sum},
                    {"size", obj.size},
                    {"retries", obj.retries},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"messageAttributes", boost::json::value_from(obj.messageAttributes)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_COUNTER_H
