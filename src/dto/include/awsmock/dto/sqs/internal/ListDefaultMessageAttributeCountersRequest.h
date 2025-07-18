//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SQS_LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SQS {

    struct ListDefaultMessageAttributeCountersRequest final : Common::BaseCounter<ListDefaultMessageAttributeCountersRequest> {

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
        long pageSize = 10;

        /**
         * Page index
         */
        long pageIndex = 0;

        /**
         * Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListDefaultMessageAttributeCountersRequest tag_invoke(boost::json::value_to_tag<ListDefaultMessageAttributeCountersRequest>, boost::json::value const &v) {
            ListDefaultMessageAttributeCountersRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListDefaultMessageAttributeCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS_REQUEST_H
