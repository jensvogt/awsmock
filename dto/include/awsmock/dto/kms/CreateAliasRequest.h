//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct CreateAliasRequest final : Common::BaseCounter<CreateAliasRequest> {

        std::string aliasName;
        std::string targetKeyId;

      private:

        friend CreateAliasRequest tag_invoke(boost::json::value_to_tag<CreateAliasRequest>, boost::json::value const &v) {
            CreateAliasRequest r;
            r.aliasName = Core::Json::GetStringValue(v, "AliasName");
            r.targetKeyId = Core::Json::GetStringValue(v, "TargetKeyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateAliasRequest const &obj) {
            jv = {{"AliasName", obj.aliasName}, {"TargetKeyId", obj.targetKeyId}};
        }
    };

}// namespace Awsmock::Dto::KMS
