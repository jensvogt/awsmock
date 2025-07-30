//
// Created by vogje01 on 12/10/23.
//

#ifndef AWSMOCK_DTO_SQS_REDRIVE_POLICY_H
#define AWSMOCK_DTO_SQS_REDRIVE_POLICY_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief SQS queue re-drive policy entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RedrivePolicy final : Common::BaseCounter<RedrivePolicy> {

        /**
         * Dead letter queue target ARN
         */
        std::string deadLetterTargetArn;

        /**
         * Maximal number of retries before the message will be sent to the DQL
         */
        long maxReceiveCount = 0;

      private:

        friend RedrivePolicy tag_invoke(boost::json::value_to_tag<RedrivePolicy>, boost::json::value const &v) {
            RedrivePolicy r;
            r.deadLetterTargetArn = Core::Json::GetStringValue(v, "DeadLetterTargetArn");
            r.maxReceiveCount = Core::Json::GetLongValue(v, "MaxReceiveCount");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RedrivePolicy const &obj) {
            jv = {
                    {"DeadLetterTargetArn", obj.deadLetterTargetArn},
                    {"MaxReceiveCount", obj.maxReceiveCount},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_REDRIVE_POLICY_H
