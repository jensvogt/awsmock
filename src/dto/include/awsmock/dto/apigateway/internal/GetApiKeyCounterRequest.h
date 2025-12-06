//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_GET_API_KEY_COUNTER_REQUEST_H
#define AWSMOCK_DTO_API_GATEWAY_GET_API_KEY_COUNTER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief Returns a single API key
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetApiKeyCounterRequest final : Common::BaseCounter<GetApiKeyCounterRequest> {

        /**
         * API Key ID
         */
        std::string id;

      private:

        friend GetApiKeyCounterRequest tag_invoke(boost::json::value_to_tag<GetApiKeyCounterRequest>, boost::json::value const &v) {
            GetApiKeyCounterRequest r;
            r.id = Core::Json::GetStringValue(v, "id");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetApiKeyCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"id", obj.id},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_GET_API_KEY_COUNTER_REQUEST_H
