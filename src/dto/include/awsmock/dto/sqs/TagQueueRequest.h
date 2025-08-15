//
// Created by vogje01 on 22/012/2023.
//

#ifndef AWSMOCK_DTO_SQS_TAG_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_TAG_QUEUE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct TagQueueRequest final : Common::BaseCounter<TagQueueRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Tags map
         */
        std::map<std::string, std::string> tags;

      private:

        friend TagQueueRequest tag_invoke(boost::json::value_to_tag<TagQueueRequest>, boost::json::value const &v) {
            TagQueueRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TagQueueRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueUrl", obj.queueUrl},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_TAG_QUEUE_REQUEST_H
