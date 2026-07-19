//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct ListResourceTagsRequest final : Common::BaseCounter<ListResourceTagsRequest> {

        std::string keyId;

      private:

        friend ListResourceTagsRequest tag_invoke(boost::json::value_to_tag<ListResourceTagsRequest>, boost::json::value const &v) {
            ListResourceTagsRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListResourceTagsRequest const &obj) {
            jv = {{"KeyId", obj.keyId}};
        }
    };

}// namespace Awsmock::Dto::KMS
