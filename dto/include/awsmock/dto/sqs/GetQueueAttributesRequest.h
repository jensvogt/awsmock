//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_ATTRIBUTE_REQUEST_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_ATTRIBUTE_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct GetQueueAttributesRequest final : Common::BaseCounter<GetQueueAttributesRequest> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * UserAttribute names list
         */
        std::vector<std::string> attributeNames;

        /**
         * Resource
         */
        std::string resource = "SQS";

      private:

        friend GetQueueAttributesRequest tag_invoke(boost::json::value_to_tag<GetQueueAttributesRequest>, boost::json::value const &v) {
            GetQueueAttributesRequest r;
            r.queueUrl = Core::Json::GetStringValue(v, "QueueUrl");
            r.resource = Core::Json::GetStringValue(v, "Resource");
            if (Core::Json::AttributeExists(v, "AttributeNames")) {
                r.attributeNames = boost::json::value_to<std::vector<std::string>>(v.at("AttributeNames"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetQueueAttributesRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"QueueUrl", obj.queueUrl},
                    {"Resource", obj.resource},
                    {"AttributeNames", boost::json::value_from(obj.attributeNames)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_ATTRIBUTE_REQUEST_H
