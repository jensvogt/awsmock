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
     * @brief Enable API gateway key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EnableApiKeyRequest final : Common::BaseCounter<EnableApiKeyRequest> {

        /**
         * API key ID
         */
        std::string keyId;

      private:

        friend EnableApiKeyRequest tag_invoke(boost::json::value_to_tag<EnableApiKeyRequest>, boost::json::value const &v) {
            EnableApiKeyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "keyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EnableApiKeyRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyId", obj.keyId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
