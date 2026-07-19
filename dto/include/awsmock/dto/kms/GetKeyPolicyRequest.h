//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct GetKeyPolicyRequest final : Common::BaseCounter<GetKeyPolicyRequest> {

        std::string keyId;
        std::string policyName;

      private:

        friend GetKeyPolicyRequest tag_invoke(boost::json::value_to_tag<GetKeyPolicyRequest>, boost::json::value const &v) {
            GetKeyPolicyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.policyName = Core::Json::GetStringValue(v, "PolicyName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetKeyPolicyRequest const &obj) {
            jv = {{"KeyId", obj.keyId}, {"PolicyName", obj.policyName}};
        }
    };

}// namespace Awsmock::Dto::KMS
