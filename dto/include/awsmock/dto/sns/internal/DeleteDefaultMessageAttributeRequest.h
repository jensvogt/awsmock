//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace Awsmock::Dto::SNS {

    struct DeleteDefaultMessageAttributeRequest final : Common::BaseCounter<DeleteDefaultMessageAttributeRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Name
         */
        std::string name;

      private:

        friend DeleteDefaultMessageAttributeRequest tag_invoke(boost::json::value_to_tag<DeleteDefaultMessageAttributeRequest>, boost::json::value const &v) {
            DeleteDefaultMessageAttributeRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.name = Core::Json::GetStringValue(v, "name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteDefaultMessageAttributeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"name", obj.name},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
