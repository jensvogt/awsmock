//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct DeleteAliasRequest final : Common::BaseCounter<DeleteAliasRequest> {

        std::string aliasName;

      private:

        friend DeleteAliasRequest tag_invoke(boost::json::value_to_tag<DeleteAliasRequest>, boost::json::value const &v) {
            DeleteAliasRequest r;
            r.aliasName = Core::Json::GetStringValue(v, "AliasName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteAliasRequest const &obj) {
            jv = {{"AliasName", obj.aliasName}};
        }
    };

}// namespace Awsmock::Dto::KMS
