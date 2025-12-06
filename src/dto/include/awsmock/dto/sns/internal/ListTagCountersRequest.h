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

        friend ListTagCountersRequest tag_invoke(boost::json::value_to_tag<ListTagCountersRequest>, boost::json::value const &v) {
            ListTagCountersRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTagCountersRequest const &obj) {
            jv = {
                    {"topicArn", obj.topicArn},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_TAG_COUNTERS_REQUEST_H
