//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>
#include <vector>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct UntagResourceRequest final : Common::BaseCounter<UntagResourceRequest> {

        std::string keyId;
        std::vector<std::string> tagKeys;

      private:

        friend UntagResourceRequest tag_invoke(boost::json::value_to_tag<UntagResourceRequest>, boost::json::value const &v) {
            UntagResourceRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            if (Core::Json::AttributeExists(v, "TagKeys")) {
                for (const auto &k: v.at("TagKeys").as_array()) {
                    r.tagKeys.push_back(k.as_string().c_str());
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UntagResourceRequest const &obj) {
            jv = {{"KeyId", obj.keyId}};
        }
    };

}// namespace Awsmock::Dto::KMS
