//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_MESSAGES_REQUEST_H
#define AWSMOCK_DTO_SNS_LIST_MESSAGES_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief List messages request
     *
     * @par
     * This is not an AWS SDK request, but only available in awsmock
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListMessagesRequest final : Common::BaseCounter<ListMessagesRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Page size
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

        friend ListMessagesRequest tag_invoke(boost::json::value_to_tag<ListMessagesRequest>, boost::json::value const &v) {
            ListMessagesRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListMessagesRequest const &obj) {
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

#endif// AWSMOCK_DTO_SNS_PUBLISH_REQUEST_H
