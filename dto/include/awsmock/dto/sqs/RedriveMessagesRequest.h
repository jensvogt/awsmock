//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief Redrive messages request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RedriveMessagesRequest final : Common::BaseCounter<RedriveMessagesRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

      private:

        friend RedriveMessagesRequest tag_invoke(boost::json::value_to_tag<RedriveMessagesRequest>, boost::json::value const &v) {
            RedriveMessagesRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "QueueArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RedriveMessagesRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueArn", obj.queueArn},
            };
        }
    };
}// namespace Awsmock::Dto::SQS
