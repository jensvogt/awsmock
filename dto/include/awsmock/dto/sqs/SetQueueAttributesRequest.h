//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_PUT_QUEUE_ATTRIBUTE_REQUEST_H
#define AWSMOCK_DTO_SQS_PUT_QUEUE_ATTRIBUTE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct SetQueueAttributesRequest final : Common::BaseCounter<SetQueueAttributesRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Attributes
         */
        std::map<std::string, std::string> attributes;

      private:

        friend SetQueueAttributesRequest tag_invoke(boost::json::value_to_tag<SetQueueAttributesRequest>, boost::json::value const &v) {
            SetQueueAttributesRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            if (Core::Json::AttributeExists(v, "Attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("Attributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SetQueueAttributesRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueUrl", obj.queueUrl},
                    {"attributes", boost::json::value_from(obj.attributes)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_PUT_QUEUE_ATTRIBUTE_REQUEST_H
