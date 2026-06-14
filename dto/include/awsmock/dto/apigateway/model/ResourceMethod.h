//
// Created by vogje01 on 6/7/26.
//

#pragma once

// Awsmock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief API gateway resource method
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ResourceMethod final : Common::BaseObject<ResourceMethod> {

        /**
         * @brief Is an API key required flag
         */
        bool apiKeyRequired;

        /**
         * @brief HTTP method
         */
        std::string httpMethod;

        /**
         * @brief Integration type: AWS, AWS_PROXY, HTTP, HTTP_PROXY, MOCK
         */
        std::string integrationType;

        /**
         * @brief Integration URI (Lambda ARN path or HTTP URL)
         */
        std::string integrationUri;

        /**
         * @brief HTTP method to use when calling the backend
         */
        std::string integrationHttpMethod;

      private:

        friend ResourceMethod tag_invoke(boost::json::value_to_tag<ResourceMethod>, boost::json::value const &v) {
            ResourceMethod r;
            r.apiKeyRequired = Core::Json::GetBoolValue(v, "apiKeyRequired");
            r.httpMethod = Core::Json::GetStringValue(v, "httpMethod");
            r.integrationType = Core::Json::GetStringValue(v, "integrationType");
            r.integrationUri = Core::Json::GetStringValue(v, "integrationUri");
            r.integrationHttpMethod = Core::Json::GetStringValue(v, "integrationHttpMethod");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ResourceMethod const &obj) {
            jv = {
                    {"apiKeyRequired", obj.apiKeyRequired},
                    {"httpMethod", obj.httpMethod},
                    {"integrationType", obj.integrationType},
                    {"integrationUri", obj.integrationUri},
                    {"integrationHttpMethod", obj.integrationHttpMethod},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
