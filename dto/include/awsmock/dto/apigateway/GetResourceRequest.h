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
     * @brief Get REST API resource request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetResourceRequest final : Common::BaseCounter<GetResourceRequest> {

        /**
         * @brief Rest API ID
         */
        std::string restApiId{};

      private:

        friend GetResourceRequest tag_invoke(boost::json::value_to_tag<GetResourceRequest>, boost::json::value const &v) {
            GetResourceRequest r;
            r.restApiId = Core::Json::GetStringValue(v, "restApiId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetResourceRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"restApiId", obj.restApiId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
