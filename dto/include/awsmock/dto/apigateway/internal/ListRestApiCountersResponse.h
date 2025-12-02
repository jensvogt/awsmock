//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_LIST_REST_API_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_API_GATEWAY_LIST_REST_API_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/RestApi.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief List REST API counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListRestApiCountersResponse final : Common::BaseCounter<ListRestApiCountersResponse> {

        /**
         * REST APIs
         */
        std::vector<RestApi> restApis;

        /**
         * User entities
         */
        long total{};

      private:

        friend ListRestApiCountersResponse tag_invoke(boost::json::value_to_tag<ListRestApiCountersResponse>, boost::json::value const &v) {
            ListRestApiCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "restApis")) {
                r.restApis = boost::json::value_to<std::vector<RestApi>>(v.at("restApis"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListRestApiCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"restApis", boost::json::value_from(obj.restApis)},
            };
        }
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_LIST_REST_API_COUNTERS_RESPONSE_H
