//
// Created by vogje01 on 07/08/2026
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Create usage plan key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateUsagePlanKeyRequest final : Common::BaseCounter<CreateUsagePlanKeyRequest> {

        /**
         * Usage plan ID (from path)
         */
        std::string usagePlanId;

        /**
         * API key ID to associate
         */
        std::string keyId;

        /**
         * Key type (always "API_KEY")
         */
        std::string keyType{"API_KEY"};

      private:

        friend CreateUsagePlanKeyRequest tag_invoke(boost::json::value_to_tag<CreateUsagePlanKeyRequest>, boost::json::value const &v) {
            CreateUsagePlanKeyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "keyId");
            r.keyType = Core::Json::GetStringValue(v, "keyType");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUsagePlanKeyRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"usagePlanId", obj.usagePlanId},
                    {"keyId", obj.keyId},
                    {"keyType", obj.keyType},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
