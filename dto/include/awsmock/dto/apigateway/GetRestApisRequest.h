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
     * @brief Get REST APis request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetRestApisRequest final : Common::BaseCounter<GetRestApisRequest> {

        /**
         * @brief Position of the first item
         */
        long position{};

        /**
         * Maximal number of items
         */
        long limit{};

      private:

        friend GetRestApisRequest tag_invoke(boost::json::value_to_tag<GetRestApisRequest>, boost::json::value const &v) {
            GetRestApisRequest r;
            r.position = Core::Json::GetLongValue(v, "position");
            r.limit = Core::Json::GetLongValue(v, "limit");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetRestApisRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"position", obj.position},
                    {"limit", obj.limit},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
