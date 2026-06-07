//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

    struct PurgeTopicRequest final : Common::BaseCounter<PurgeTopicRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

      private:

        friend PurgeTopicRequest tag_invoke(boost::json::value_to_tag<PurgeTopicRequest>, boost::json::value const &v) {
            PurgeTopicRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PurgeTopicRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
