//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_ATTRIBUTE_RESPONSE_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_ATTRIBUTE_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    struct GetQueueAttributesResponse final : Common::BaseCounter<GetQueueAttributesResponse> {

        /**
         * Attributes
         */
        std::vector<std::pair<std::string, std::string>> attributes;

        /**
         * Resource
         */
        std::string resource = "SQS";

      private:

        friend GetQueueAttributesResponse tag_invoke(boost::json::value_to_tag<GetQueueAttributesResponse>, boost::json::value const &v) {
            GetQueueAttributesResponse r;
            r.resource = Core::Json::GetStringValue(v, "Resource");
            if (Core::Json::AttributeExists(v, "Attributes")) {
                r.attributes = boost::json::value_to<std::vector<std::pair<std::string, std::string>>>(v.at("Attributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetQueueAttributesResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Resource", obj.resource}};

            if (!obj.attributes.empty()) {
                boost::json::object jvAttribute;
                for (const auto &[fst, snd]: obj.attributes) {
                    jvAttribute[fst] = snd;
                }
                jv.as_object()["Attributes"] = jvAttribute;
            }
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_ATTRIBUTE_RESPONSE_H
