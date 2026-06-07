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
     * @brief Delete REST API request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteRestApiRequest final : Common::BaseCounter<DeleteRestApiRequest> {

        /**
         * REST API ID
         */
        std::string restApiId;

      private:

        friend DeleteRestApiRequest tag_invoke(boost::json::value_to_tag<DeleteRestApiRequest>, boost::json::value const &v) {
            DeleteRestApiRequest r;
            r.restApiId = Core::Json::GetStringValue(v, "restApiId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteRestApiRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"restApiId", obj.restApiId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
