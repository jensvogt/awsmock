//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_TAG_COUNTER_H
#define AWSMOCK_DTO_SQS_TAG_COUNTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct TagCounter final : Common::BaseCounter<TagCounter> {

        /**
         * Key
         */
        std::string tagKey;

        /**
         * Value
         */
        std::string tagValue;

      private:

        friend TagCounter tag_invoke(boost::json::value_to_tag<TagCounter>, boost::json::value const &v) {
            TagCounter r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.tagKey = v.at("tagKey").as_string();
            r.tagValue = v.at("tagValue").as_string();
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TagCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"tagKey", obj.tagKey},
                    {"tagValue", obj.tagValue},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_TAG_COUNTER_H
