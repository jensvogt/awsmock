//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/common/SortColumn.h"


#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::SNS {

    struct ListTopicCountersRequest final : Common::BaseCounter<ListTopicCountersRequest> {

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        long pageSize;

        /**
         * Page index
         */
        long pageIndex;

        /**
         * Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListTopicCountersRequest tag_invoke(boost::json::value_to_tag<ListTopicCountersRequest>, boost::json::value const &v) {
            ListTopicCountersRequest r;
            r.prefix = v.at("prefix").as_string();
            r.pageSize = v.at("pageSize").as_int64();
            r.pageIndex = v.at("pageIndex").as_int64();
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTopicCountersRequest const &obj) {
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

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_REQUEST_H
