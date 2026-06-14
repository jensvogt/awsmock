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

    struct DeleteTopicRequest final : Common::BaseCounter<DeleteTopicRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

      private:

        friend DeleteTopicRequest tag_invoke(boost::json::value_to_tag<DeleteTopicRequest>, boost::json::value const &v) {
            DeleteTopicRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteTopicRequest const &obj) {
            jv = {
                    {"topicArn", obj.topicArn},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
