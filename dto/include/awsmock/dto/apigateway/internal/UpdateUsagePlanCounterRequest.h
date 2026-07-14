//
// Created by vogje01 on 07/14/2026
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apigateway/model/UsagePlan.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Update a usage plan request (management internal)
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateUsagePlanCounterRequest final : Common::BaseCounter<UpdateUsagePlanCounterRequest> {

        /**
         * Usage plan to update
         */
        UsagePlan usagePlan;

      private:

        friend UpdateUsagePlanCounterRequest tag_invoke(boost::json::value_to_tag<UpdateUsagePlanCounterRequest>, boost::json::value const &v) {
            UpdateUsagePlanCounterRequest r;
            if (Core::Json::AttributeExists(v, "usagePlan")) {
                r.usagePlan = boost::json::value_to<UsagePlan>(v.at("usagePlan"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateUsagePlanCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"usagePlan", boost::json::value_from(obj.usagePlan)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
