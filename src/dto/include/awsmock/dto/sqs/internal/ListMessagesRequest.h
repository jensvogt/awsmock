//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_MESSAGES_REQUEST_H
#define AWSMOCK_DTO_SQS_LIST_MESSAGES_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS messages request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListMessagesRequest final : Common::BaseCounter<ListMessagesRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

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
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListMessagesRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_MESSAGES_REQUEST_H
