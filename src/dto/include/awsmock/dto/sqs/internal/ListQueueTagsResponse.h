//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS queues tags response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListQueueTagsResponse final : Common::BaseCounter<ListQueueTagsResponse> {

        /**
         * @brief List of queues tags
         */
        std::map<std::string, std::string> tags;

        /**
         * @brief Total number of tags
         */
        long total{};

      private:

        friend ListQueueTagsResponse tag_invoke(boost::json::value_to_tag<ListQueueTagsResponse>, boost::json::value const &v) {
            ListQueueTagsResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueueTagsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_RESPONSE_H
