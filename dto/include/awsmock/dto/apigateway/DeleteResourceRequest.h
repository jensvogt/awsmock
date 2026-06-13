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
     * @brief Delete an API gateway resource
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteResourceRequest final : Common::BaseCounter<DeleteResourceRequest> {

        /**
         * REST API ID
         */
        std::string restApiId;

        /**
         * Resource ID
         */
        std::string resourceId;

      private:

        friend DeleteResourceRequest tag_invoke(boost::json::value_to_tag<DeleteResourceRequest>, boost::json::value const &v) {
            DeleteResourceRequest r;
            r.restApiId = Core::Json::GetStringValue(v, "restApiId");
            r.resourceId = Core::Json::GetStringValue(v, "resourceId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteResourceRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"restApiId", obj.restApiId},
                    {"resourceId", obj.resourceId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
