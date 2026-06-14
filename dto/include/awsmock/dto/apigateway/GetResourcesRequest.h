//
// Created by vogje01 on 01/09/2025
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Get REST API resources request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetResourcesRequest final : Common::BaseCounter<GetResourcesRequest> {

        /**
         * @brief Position of the first item
         */
        std::string restApiId{};

      private:

        friend GetResourcesRequest tag_invoke(boost::json::value_to_tag<GetResourcesRequest>, boost::json::value const &v) {
            GetResourcesRequest r;
            r.restApiId = Core::Json::GetStringValue(v, "restApiId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetResourcesRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"restApiId", obj.restApiId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
