//
// Created by vogje01 on 6/7/26.
//

#pragma once

// Awsmock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief API gateway resource method
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ResourceMethod final : Common::BaseCounter<ResourceMethod> {

        /**
         * @brief Is an API key required flag
         */
        bool apiKeyRequired;

        /**
         * @brief HTTP method
         */
        std::string httpMethod;

      private:

        friend ResourceMethod tag_invoke(boost::json::value_to_tag<ResourceMethod>, boost::json::value const &v) {
            ResourceMethod r;
            r.apiKeyRequired = Core::Json::GetBoolValue(v, "apiKeyRequired");
            r.httpMethod = Core::Json::GetStringValue(v, "httpMethod");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ResourceMethod const &obj) {
            jv = {
                    {"apiKeyRequired", obj.apiKeyRequired},
                    {"httpMethod", obj.httpMethod},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
