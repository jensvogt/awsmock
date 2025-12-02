//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_UNTAG_RESOURCE_REQUEST_H
#define AWSMOCK_DTO_SNS_UNTAG_RESOURCE_REQUEST_H

// C++ standard includes
#include <string>

// Awsmock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct UntagResourceRequest final : Common::BaseCounter<UntagResourceRequest> {

        /**
         * Resource ARN (topic ARN)
         */
        std::string resourceArn;

        /**
         * Tags map
         */
        std::vector<std::string> tags;

      private:

        friend UntagResourceRequest tag_invoke(boost::json::value_to_tag<UntagResourceRequest>, boost::json::value const &v) {
            UntagResourceRequest r;
            r.resourceArn = Core::Json::GetStringValue(v, "ResourceArn");
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::vector<std::string>>(v.at("Tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UntagResourceRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"resourceArn", obj.resourceArn},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_UNTAG_RESOURCE_REQUEST_H
