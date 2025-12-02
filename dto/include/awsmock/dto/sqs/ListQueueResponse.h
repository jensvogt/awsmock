//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/Queue.h>
#include <awsmock/entity/sqs/Queue.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS queues response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListQueuesResponse final : Common::BaseCounter<ListQueuesResponse> {

        /**
         * List of queues
         */
        std::vector<std::string> queueUrls;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Total number of queues
         */
        long total{};

      private:

        friend ListQueuesResponse tag_invoke(boost::json::value_to_tag<ListQueuesResponse>, boost::json::value const &v) {
            ListQueuesResponse r;
            r.total = Core::Json::GetLongValue(v, "Total");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            if (Core::Json::AttributeExists(v, "QueueUrls")) {
                r.queueUrls = boost::json::value_to<std::vector<std::string>>(v.at("QueueUrls"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueuesResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Total", obj.total},
                    {"NextToken", obj.nextToken},
                    {"QueueUrls", boost::json::value_from(obj.queueUrls)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_RESPONSE_H
