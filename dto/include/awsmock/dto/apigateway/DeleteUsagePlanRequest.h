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
     * @brief Delete usage plan request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteUsagePlanRequest final : Common::BaseCounter<DeleteUsagePlanRequest> {

        /**
         * Usage plan ID
         */
        std::string usagePlanId;

      private:

        friend DeleteUsagePlanRequest tag_invoke(boost::json::value_to_tag<DeleteUsagePlanRequest>, boost::json::value const &v) {
            DeleteUsagePlanRequest r;
            r.usagePlanId = Core::Json::GetStringValue(v, "usagePlanId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteUsagePlanRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"usagePlanId", obj.usagePlanId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
