//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct ListAliasesRequest final : Common::BaseCounter<ListAliasesRequest> {

        // Optional: filter by key ID
        std::string keyId;

      private:

        friend ListAliasesRequest tag_invoke(boost::json::value_to_tag<ListAliasesRequest>, boost::json::value const &v) {
            ListAliasesRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListAliasesRequest const &obj) {
            jv = {{"KeyId", obj.keyId}};
        }
    };

}// namespace Awsmock::Dto::KMS
