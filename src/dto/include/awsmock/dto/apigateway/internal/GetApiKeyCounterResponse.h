//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_GET_API_KEY_COUNTER_RESPONSE_H
#define AWSMOCK_DTO_API_GATEWAY_GET_API_KEY_COUNTER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/Key.h>
#include <awsmock/dto/cognito/model/UserCounter.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief Get single API keys response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetApiKeyCounterResponse final : Common::BaseCounter<GetApiKeyCounterResponse> {

        /**
         * API key
         */
        Key apiKey;

      private:

        friend GetApiKeyCounterResponse tag_invoke(boost::json::value_to_tag<GetApiKeyCounterResponse>, boost::json::value const &v) {
            GetApiKeyCounterResponse r;
            if (Core::Json::AttributeExists(v, "apiKey")) {
                r.apiKey = boost::json::value_to<Key>(v.at("apiKey"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetApiKeyCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"apiKey", boost::json::value_from(obj.apiKey)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_GET_API_KEY_COUNTER_RESPONSE_H
