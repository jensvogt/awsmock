//
// Created by vogje01 on 01/09/2025
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/RestApi.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Get REST APIs request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetRestApisResponse final : Common::BaseCounter<GetRestApisResponse> {

        /**
         * @brief List of rest APIs
         */
        std::vector<RestApi> restApis{};

      private:

        friend GetRestApisResponse tag_invoke(boost::json::value_to_tag<GetRestApisResponse>, boost::json::value const &v) {
            GetRestApisResponse r;
            r.restApis = boost::json::value_to<std::vector<RestApi>>(v.at("item"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetRestApisResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"item", boost::json::value_from(obj.restApis)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
