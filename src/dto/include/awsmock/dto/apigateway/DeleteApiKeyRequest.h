//
// Created by vogje01 on 01/09/2025
//

#ifndef AWSMOCK_DTO_API_GATEWAY_DELETE_API_KEY_REQUEST_H
#define AWSMOCK_DTO_API_GATEWAY_DELETE_API_KEY_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief Delete API gateway key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteApiKeyRequest final : Common::BaseCounter<DeleteApiKeyRequest> {

        /**
         * API key (id of the key)
         */
        std::string apiKey;

      private:

        friend DeleteApiKeyRequest tag_invoke(boost::json::value_to_tag<DeleteApiKeyRequest>, boost::json::value const &v) {
            DeleteApiKeyRequest r;
            r.apiKey = Core::Json::GetStringValue(v, "apiKey");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteApiKeyRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"apiKey", obj.apiKey},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_DELETE_API_KEY_REQUEST_H
