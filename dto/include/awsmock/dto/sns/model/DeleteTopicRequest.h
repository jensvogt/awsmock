//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_DELETE_TOPIC_REQUEST_H
#define AWSMOCK_DTO_SNS_DELETE_TOPIC_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::SNS {

    struct DeleteTopicRequest final : Common::BaseObject<DeleteTopicRequest> {

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
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
            };
        }
    };

}// namespace Awsmock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_DELETE_TOPIC_REQUEST_H
