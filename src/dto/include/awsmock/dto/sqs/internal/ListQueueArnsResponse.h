//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_ARNS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_ARNS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS queue ARNs response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListQueueArnsResponse final : Common::BaseCounter<ListQueueArnsResponse> {

        /**
         * List of queues ARNs
         */
        std::vector<std::string> queueArns;

      private:

        friend ListQueueArnsResponse tag_invoke(boost::json::value_to_tag<ListQueueArnsResponse>, boost::json::value const &v) {
            ListQueueArnsResponse r;
            if (Core::Json::AttributeExists(v, "QueueArn")) {
                r.queueArns = boost::json::value_to<std::vector<std::string>>(v.at("QueueArn"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueueArnsResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueArn", boost::json::value_from(obj.queueArns)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_ARNS_RESPONSE_H
