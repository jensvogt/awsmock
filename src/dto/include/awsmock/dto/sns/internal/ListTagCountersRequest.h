//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_TAG_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SNS_LIST_TAG_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SNS {

    struct ListTagCountersRequest final : Common::BaseCounter<ListTagCountersRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * TopicArn
         */
        std::string topicArn;

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

        friend ListTagCountersRequest tag_invoke(boost::json::value_to_tag<ListTagCountersRequest>, boost::json::value const &v) {
            ListTagCountersRequest r;
            r.prefix = v.at("prefix").as_string();
            r.pageSize = static_cast<int>(v.at("pageSize").as_int64());
            r.pageIndex = static_cast<int>(v.at("pageIndex").as_int64());
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTagCountersRequest const &obj) {
            jv = {
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_TAG_COUNTERS_REQUEST_H
