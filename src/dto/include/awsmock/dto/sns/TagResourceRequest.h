//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_TAG_RESOURCE_REQUEST_H
#define AWSMOCK_DTO_SNS_TAG_RESOURCE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// Awsmock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct TagResourceRequest final : Common::BaseCounter<TagResourceRequest> {

        /**
         * Resource ARN (topic ARN)
         */
        std::string resourceArn;

        /**
         * Tags map
         */
        std::map<std::string, std::string> tags;

      private:

        friend TagResourceRequest tag_invoke(boost::json::value_to_tag<TagResourceRequest>, boost::json::value const &v) {
            TagResourceRequest r;
            r.resourceArn = Core::Json::GetStringValue(v, "ResourceArn");
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TagResourceRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"resourceArn", obj.resourceArn},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

    typedef std::map<std::string, std::string> TagList;

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_TAG_RESOURCE_REQUEST_H
