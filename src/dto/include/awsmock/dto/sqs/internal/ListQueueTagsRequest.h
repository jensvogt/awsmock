//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_REQUEST_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/SortColumn.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS queue tags request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListQueueTagsRequest final : Common::BaseCounter<ListQueueTagsRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Page size
         */
        long pageSize;

        /**
         * Page index
         */
        long pageIndex;

        /**
         * @brief Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListQueueTagsRequest tag_invoke(boost::json::value_to_tag<ListQueueTagsRequest>, boost::json::value const &v) {
            ListQueueTagsRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.pageSize = Core::Json::GetLongValue(v, "PageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "PageIndex");
            if (Core::Json::AttributeExists(v, "SortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("SortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueueTagsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
                    {"PageSize", obj.pageSize},
                    {"PageIndex", obj.pageIndex},
                    {"SortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_REQUEST_H
