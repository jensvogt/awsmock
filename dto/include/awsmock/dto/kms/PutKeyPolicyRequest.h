//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::KMS {

    struct PutKeyPolicyRequest final : Common::BaseCounter<PutKeyPolicyRequest> {

        std::string keyId;
        std::string policyName;
        std::string policy;

      private:

        friend PutKeyPolicyRequest tag_invoke(boost::json::value_to_tag<PutKeyPolicyRequest>, boost::json::value const &v) {
            PutKeyPolicyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.policyName = Core::Json::GetStringValue(v, "PolicyName");
            r.policy = Core::Json::GetStringValue(v, "Policy");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutKeyPolicyRequest const &obj) {
            jv = {{"KeyId", obj.keyId}, {"PolicyName", obj.policyName}, {"Policy", obj.policy}};
        }
    };

}// namespace Awsmock::Dto::KMS
