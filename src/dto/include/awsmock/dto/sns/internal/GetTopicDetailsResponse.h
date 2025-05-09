//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_TOPIC_DETAILS_RESPONSE_H
#define AWSMOCK_DTO_SQS_GET_TOPIC_DETAILS_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    using std::chrono::system_clock;

    /**
     * @brief Get topic details response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetTopicDetailsResponse final : Common::BaseCounter<GetTopicDetailsResponse> {

        /**
         * Topic name
         */
        std::string topicName;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Number of messages
         */
        long messageCount{};

        /**
         * Size of all messages
         */
        long size{};

        /**
         * Topic owner
         */
        std::string owner;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

      private:

        friend GetTopicDetailsResponse tag_invoke(boost::json::value_to_tag<GetTopicDetailsResponse>, boost::json::value const &v) {
            GetTopicDetailsResponse r;
            r.topicName = Core::Json::GetStringValue(v, "topicName");
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.messageCount = Core::Json::GetLongValue(v, "messageCount");
            r.size = Core::Json::GetLongValue(v, "size");
            r.owner = Core::Json::GetStringValue(v, "owner");
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "modified"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetTopicDetailsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicName", obj.topicName},
                    {"topicArn", obj.topicArn},
                    {"messageCount", obj.messageCount},
                    {"size", obj.size},
                    {"owner", obj.owner},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SQS_GET_TOPIC_DETAILS_RESPONSE_H
