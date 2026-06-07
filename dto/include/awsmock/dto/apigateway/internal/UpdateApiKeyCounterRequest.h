//
// Created by vogje01 on 11/25/23.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/Key.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Update API key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateApiKeyCounterRequest final : Common::BaseCounter<UpdateApiKeyCounterRequest> {

        /**
         * API key to update
         */
        Key apiKey;

      private:

        friend UpdateApiKeyCounterRequest tag_invoke(boost::json::value_to_tag<UpdateApiKeyCounterRequest>, boost::json::value const &v) {
            UpdateApiKeyCounterRequest r;
            if (Core::Json::AttributeExists(v, "apiKey")) {
                r.apiKey = boost::json::value_to<Key>(v.at("apiKey"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateApiKeyCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"apiKey", boost::json::value_from(obj.apiKey)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
