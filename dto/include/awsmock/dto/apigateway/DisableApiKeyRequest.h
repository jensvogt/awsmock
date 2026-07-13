//
// Created by vogje01 on 01/09/2025
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Disable API gateway key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DisableApiKeyRequest final : Common::BaseCounter<DisableApiKeyRequest> {

        /**
         * API key ID
         */
        std::string keyId;

      private:

        friend DisableApiKeyRequest tag_invoke(boost::json::value_to_tag<DisableApiKeyRequest>, boost::json::value const &v) {
            DisableApiKeyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "keyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DisableApiKeyRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyId", obj.keyId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
