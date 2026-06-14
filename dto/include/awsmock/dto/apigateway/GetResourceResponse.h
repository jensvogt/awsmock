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
     * @brief Get REST APIs resource response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetResourceResponse final : Common::BaseCounter<GetResourceResponse> {

        /**
         * @brief Rest APIs resources
         */
        Resource resource{};

        /**
         * @brief Current position
         */
        std::string position{};

      private:

        friend GetResourceResponse tag_invoke(boost::json::value_to_tag<GetResourceResponse>, boost::json::value const &v) {
            GetResourceResponse r;
            r.resource = boost::json::value_to<Resource>(v.at("item"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetResourceResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"item", boost::json::value_from(obj.resource)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
