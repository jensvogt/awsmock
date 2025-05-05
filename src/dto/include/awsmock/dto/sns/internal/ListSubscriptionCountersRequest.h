//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_SUBSCRIPTIONS_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SNS_LIST_SUBSCRIPTIONS_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SNS {

    struct ListSubscriptionCountersRequest final : Common::BaseCounter<ListSubscriptionCountersRequest> {

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
        long pageSize{};

        /**
         * Page index
         */
        long pageIndex{};

        /**
         * Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListSubscriptionCountersRequest tag_invoke(boost::json::value_to_tag<ListSubscriptionCountersRequest>, boost::json::value const &v) {
            ListSubscriptionCountersRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSubscriptionCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_SUBSCRIPTIONS_COUNTERS_REQUEST_H
