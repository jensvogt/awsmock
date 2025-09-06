//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_LIST_API_KEY_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_API_GATEWAY_LIST_API_KEY_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/Key.h>
#include <awsmock/dto/cognito/model/UserCounter.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief List API keys counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListApiKeyCountersResponse final : Common::BaseCounter<ListApiKeyCountersResponse> {

        /**
         * Api keys
         */
        std::vector<Key> apiKeys;

        /**
         * User entities
         */
        long total{};

      private:

        friend ListApiKeyCountersResponse tag_invoke(boost::json::value_to_tag<ListApiKeyCountersResponse>, boost::json::value const &v) {
            ListApiKeyCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "apiKeys")) {
                r.apiKeys = boost::json::value_to<std::vector<Key>>(v.at("apiKeys"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListApiKeyCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"apiKeys", boost::json::value_from(obj.apiKeys)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_LIST_API_KEY_COUNTERS_RESPONSE_H
