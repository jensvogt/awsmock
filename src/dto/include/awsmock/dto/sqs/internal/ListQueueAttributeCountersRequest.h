//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_ATTRIBUTE_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_ATTRIBUTE_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::SQS {

    struct ListQueueAttributeCountersRequest final : Common::BaseCounter<ListQueueAttributeCountersRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListQueueAttributeCountersRequest tag_invoke(boost::json::value_to_tag<ListQueueAttributeCountersRequest>, boost::json::value const &v) {
            ListQueueAttributeCountersRequest r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.prefix = v.at("prefix").as_string();
            r.pageSize = static_cast<int>(v.at("pageSize").as_int64());
            r.pageIndex = static_cast<int>(v.at("pageIndex").as_int64());
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueueAttributeCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_ATTRIBUTE_COUNTERS_REQUEST_H
