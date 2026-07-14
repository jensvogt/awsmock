//
// Created by vogje01 on 07/14/2026
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Get a single usage plan request (management internal)
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetUsagePlanCounterRequest final : Common::BaseCounter<GetUsagePlanCounterRequest> {

        /**
         * Usage plan ID
         */
        std::string usagePlanId;

      private:

        friend GetUsagePlanCounterRequest tag_invoke(boost::json::value_to_tag<GetUsagePlanCounterRequest>, boost::json::value const &v) {
            GetUsagePlanCounterRequest r;
            r.usagePlanId = Core::Json::GetStringValue(v, "usagePlanId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetUsagePlanCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"usagePlanId", obj.usagePlanId},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
