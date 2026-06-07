//
// Created by vogje01 on 11/25/23.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/RestApi.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Updates a REST API request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateRestApiCounterRequest final : Common::BaseCounter<UpdateRestApiCounterRequest> {

        /**
         * REST API to update
         */
        RestApi restApi;

      private:

        friend UpdateRestApiCounterRequest tag_invoke(boost::json::value_to_tag<UpdateRestApiCounterRequest>, boost::json::value const &v) {
            UpdateRestApiCounterRequest r;
            if (Core::Json::AttributeExists(v, "restApi")) {
                r.restApi = boost::json::value_to<RestApi>(v.at("restApi"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateRestApiCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"restApi", boost::json::value_from(obj.restApi)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
