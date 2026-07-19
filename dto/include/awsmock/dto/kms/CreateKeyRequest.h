//
// Created by vogje01 on 4/25/24.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/KeySpec.h>
#include <awsmock/dto/kms/model/KeyUsage.h>

namespace Awsmock::Dto::KMS {

    /**
     * @brief Create KMS key request
     *
     * Example:
     * @code{.json}
     * {
     *   "BypassPolicyLockoutSafetyCheck": boolean,
     *   "KeySpec": "string",
     *   "CustomKeyStoreId": "string",
     *   "Description": "string",
     *   "KeySpec": "string",
     *   "KeyUsage": "string",
     *   "MultiRegion": boolean,
     *   "Origin": "string",
     *   "Policy": "string",
     *   "Tags": [
     *     {
     *       "TagKey": "string",
     *       "TagValue": "string"
     *     }
     *   ],
     *   "XksKeyId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateKeyRequest final : Common::BaseCounter<CreateKeyRequest> {

        /**
         * Key specification
         */
        KeySpec keySpec = KeySpec::SYMMETRIC_DEFAULT;

        /**
         * Key usage
         */
        KeyUsage keyUsage = KeyUsage::ENCRYPT_DECRYPT;

        /**
         * Description
         */
        std::string description;

        /**
         * Customer key store ID
         */
        std::string customKeyStoreId;

        /**
         * Multi region
         */
        bool multiRegion = false;

        /**
         * Bypass policy lockout safety check
         */
        bool bypassPolicyLockoutSafetyCheck = true;

        /**
         * Origin
         */
        std::string origin;

        /**
         * Policy
         */
        std::string policy;

        /**
         * Xks key ID
         */
        std::string xksKeyId;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend CreateKeyRequest tag_invoke(boost::json::value_to_tag<CreateKeyRequest>, boost::json::value const &v) {
            CreateKeyRequest r;
            r.keySpec = KeySpecFromString(Core::Json::GetStringValue(v, "KeySpec"));
            r.keyUsage = KeyUsageFromString(Core::Json::GetStringValue(v, "KeyUsage"));
            r.description = Core::Json::GetStringValue(v, "Description");
            r.customKeyStoreId = Core::Json::GetStringValue(v, "CustomKeyStoreId");
            r.multiRegion = Core::Json::GetBoolValue(v, "MultiRegion");
            r.bypassPolicyLockoutSafetyCheck = Core::Json::GetBoolValue(v, "BypassPolicyLockoutSafetyCheck");
            r.origin = Core::Json::GetStringValue(v, "Origin");
            r.policy = Core::Json::GetStringValue(v, "Policy");
            r.xksKeyId = Core::Json::GetStringValue(v, "XksKeyId");
            // AWS SDK sends Tags as [{TagKey,TagValue}] array; handle both array and object forms
            if (Core::Json::AttributeExists(v, "Tags")) {
                const auto &tagsVal = v.at("Tags");
                if (tagsVal.is_array()) {
                    for (const auto &tag: tagsVal.as_array()) {
                        const std::string k = Core::Json::GetStringValue(tag, "TagKey");
                        const std::string val = Core::Json::GetStringValue(tag, "TagValue");
                        if (!k.empty()) {
                            r.tags[k] = val;
                        }
                    }
                } else if (tagsVal.is_object()) {
                    r.tags = boost::json::value_to<std::map<std::string, std::string>>(tagsVal);
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateKeyRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"keySpec", KeySpecToString(obj.keySpec)},
                    {"keyUsage", KeyUsageToString(obj.keyUsage)},
                    {"description", obj.description},
                    {"customKeyStoreId", obj.customKeyStoreId},
                    {"multiRegion", obj.multiRegion},
                    {"bypassPolicyLockoutSafetyCheck", obj.bypassPolicyLockoutSafetyCheck},
                    {"origin", obj.bypassPolicyLockoutSafetyCheck},
                    {"policy", obj.policy},
                    {"xksKeyId", obj.xksKeyId},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };
}// namespace Awsmock::Dto::KMS
