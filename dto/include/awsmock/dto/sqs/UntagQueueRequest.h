//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_UNTAG_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_UNTAG_QUEUE_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct UntagQueueRequest final : Common::BaseCounter<UntagQueueRequest> {

        /**
         * Queue Url
         */
        std::string queueUrl;

        /**
         * Tags map
         */
        std::vector<std::string> tags;

      private:

        friend UntagQueueRequest tag_invoke(boost::json::value_to_tag<UntagQueueRequest>, boost::json::value const &v) {
            UntagQueueRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::vector<std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UntagQueueRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueUrl", obj.queueUrl},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };
};// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_UNTAG_QUEUE_REQUEST_H
