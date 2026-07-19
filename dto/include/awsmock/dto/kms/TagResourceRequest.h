//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <map>
#include <string>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct TagResourceRequest final : Common::BaseCounter<TagResourceRequest> {

        std::string keyId;
        std::map<std::string, std::string> tags;

      private:

        friend TagResourceRequest tag_invoke(boost::json::value_to_tag<TagResourceRequest>, boost::json::value const &v) {
            TagResourceRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            if (Core::Json::AttributeExists(v, "Tags")) {
                for (const auto &t: v.at("Tags").as_array()) {
                    const std::string k = Core::Json::GetStringValue(t, "TagKey");
                    const std::string val = Core::Json::GetStringValue(t, "TagValue");
                    if (!k.empty()) r.tags[k] = val;
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TagResourceRequest const &obj) {
            jv = {{"KeyId", obj.keyId}};
        }
    };

}// namespace Awsmock::Dto::KMS
