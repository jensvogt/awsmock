//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_CREATE_TOPIC_REQUEST_H
#define AWSMOCK_DTO_SNS_CREATE_TOPIC_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct CreateTopicRequest final : Common::BaseCounter<CreateTopicRequest> {

        /**
         * Name
         */
        std::string topicName;

        /**
         * Owner
         */
        std::string owner;

      private:

        friend CreateTopicRequest tag_invoke(boost::json::value_to_tag<CreateTopicRequest>, boost::json::value const &v) {
            CreateTopicRequest r;
            r.topicName = Core::Json::GetStringValue(v, "topicName");
            r.owner = Core::Json::GetStringValue(v, "owner");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateTopicRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicName", obj.topicName},
                    {"owner", obj.owner},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_CREATE_TOPIC_REQUEST_H
