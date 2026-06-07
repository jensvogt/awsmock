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

    struct GetTopicAttributesRequest final : Common::BaseCounter<GetTopicAttributesRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

      private:

        friend GetTopicAttributesRequest tag_invoke(boost::json::value_to_tag<GetTopicAttributesRequest>, boost::json::value const &v) {
            GetTopicAttributesRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "TopicArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetTopicAttributesRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
