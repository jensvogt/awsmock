//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/AttributeCounter.h>

namespace AwsMock::Dto::SQS {

    struct ListDefaultMessageAttributeCountersResponse final : Common::BaseCounter<ListDefaultMessageAttributeCountersResponse> {

        /**
         * List of attribute counters
         */
        std::map<std::string, MessageAttribute> attributeCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListDefaultMessageAttributeCountersResponse tag_invoke(boost::json::value_to_tag<ListDefaultMessageAttributeCountersResponse>, boost::json::value const &v) {
            ListDefaultMessageAttributeCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "attributeCounters")) {
                r.attributeCounters = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("attributeCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListDefaultMessageAttributeCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"attributeCounters", boost::json::value_from(obj.attributeCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SNS_LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS_RESPONSE_H
