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
     * @brief Get REST APIs reosurces response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetResourcesResponse final : Common::BaseCounter<GetResourcesResponse> {

        /**
         * @brief List of rest APIs resources
         */
        std::vector<Resource> resources{};

        /**
         * @brief Current position
         */
        std::string position{};

      private:

        friend GetResourcesResponse tag_invoke(boost::json::value_to_tag<GetResourcesResponse>, boost::json::value const &v) {
            GetResourcesResponse r;
            r.resources = boost::json::value_to<std::vector<Resource>>(v.at("item"));
            r.position = Core::Json::GetStringValue(v, "position");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetResourcesResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"item", boost::json::value_from(obj.resources)},
                    {"position", obj.position},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
