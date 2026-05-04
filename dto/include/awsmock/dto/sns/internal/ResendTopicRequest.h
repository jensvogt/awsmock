//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_RESEND_TOPIC_REQUEST_H
#define AWSMOCK_DTO_SNS_RESEND_TOPIC_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief Resend all message in the topic
     *
     * @par
     * This is not an AWS SDK request, but only available in awsmock
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ResendTopicRequest final : Common::BaseCounter<ResendTopicRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

      private:

        friend ResendTopicRequest tag_invoke(boost::json::value_to_tag<ResendTopicRequest>, boost::json::value const &v) {
            ResendTopicRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ResendTopicRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_RESEND_TOPIC_REQUEST_H
