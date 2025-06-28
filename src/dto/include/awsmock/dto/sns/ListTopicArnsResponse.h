//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_TOPIC_ARNS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_TOPIC_ARNS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief List SQS queue ARNs response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTopicArnsResponse final : Common::BaseCounter<ListTopicArnsResponse> {

        /**
         * List of topic ARNs
         */
        std::vector<std::string> topicArns;

      private:

        friend ListTopicArnsResponse tag_invoke(boost::json::value_to_tag<ListTopicArnsResponse>, boost::json::value const &v) {
            ListTopicArnsResponse r;
            r.topicArns = boost::json::value_to<std::vector<std::string>>(v.at("topicArns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTopicArnsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArns", boost::json::value_from(obj.topicArns)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_TOPIC_ARNS_RESPONSE_H
