//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>
#include <vector>

#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/KeySpec.h>

namespace Awsmock::Dto::KMS {

    struct GenerateDataKeyRequest final : Common::BaseCounter<GenerateDataKeyRequest> {

        std::string keyId;
        int numberOfBytes = 32;
        KeySpec keySpec = KeySpec::SYMMETRIC_DEFAULT;
        std::vector<std::string> grantTokens;

      private:

        friend GenerateDataKeyRequest tag_invoke(boost::json::value_to_tag<GenerateDataKeyRequest>, boost::json::value const &v) {
            GenerateDataKeyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.keySpec = KeySpecFromString(Core::Json::GetStringValue(v, "KeySpec"));
            if (Core::Json::AttributeExists(v, "NumberOfBytes")) {
                r.numberOfBytes = static_cast<int>(v.at("NumberOfBytes").as_int64());
            }
            if (Core::Json::AttributeExists(v, "GrantTokens")) {
                r.grantTokens = boost::json::value_to<std::vector<std::string>>(v.at("GrantTokens"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GenerateDataKeyRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"NumberOfBytes", obj.numberOfBytes},
                    {"KeySpec", KeySpecToString(obj.keySpec)},
            };
        }
    };

}// namespace Awsmock::Dto::KMS
