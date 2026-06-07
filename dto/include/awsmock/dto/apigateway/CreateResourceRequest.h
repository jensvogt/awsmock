//
// Created by vogje01 on 01/09/2025
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Create API gateway key request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateResourceRequest final : Common::BaseCounter<CreateResourceRequest> {

        /**
         * ID of the corresponding REST API
         */
        std::string restApiId;

        /**
         * Path part
         */
        std::string pathPart;

      private:

        friend CreateResourceRequest tag_invoke(boost::json::value_to_tag<CreateResourceRequest>, boost::json::value const &v) {
            CreateResourceRequest r;
            r.restApiId = Core::Json::GetStringValue(v, "restApiId");
            r.pathPart = Core::Json::GetStringValue(v, "pathPart");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateResourceRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"restApiId", obj.restApiId},
                    {"pathPart", obj.pathPart},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
