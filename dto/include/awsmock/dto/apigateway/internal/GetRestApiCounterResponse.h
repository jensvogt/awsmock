//
// Created by vogje01 on 11/25/23.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/RestApi.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Get a single REST API response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetRestApiCounterResponse final : Common::BaseCounter<GetRestApiCounterResponse> {

        /**
         * API key
         */
        RestApi restApi;

      private:

        friend GetRestApiCounterResponse tag_invoke(boost::json::value_to_tag<GetRestApiCounterResponse>, boost::json::value const &v) {
            GetRestApiCounterResponse r;
            if (Core::Json::AttributeExists(v, "restApi")) {
                r.restApi = boost::json::value_to<RestApi>(v.at("restApi"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetRestApiCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"restApi", boost::json::value_from(obj.restApi)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
