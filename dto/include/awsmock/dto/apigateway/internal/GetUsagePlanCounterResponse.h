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
     * @brief Get a single usage plan response (management internal)
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetUsagePlanCounterResponse final : Common::BaseCounter<GetUsagePlanCounterResponse> {

        /**
         * Usage plan
         */
        UsagePlan usagePlan;

      private:

        friend GetUsagePlanCounterResponse tag_invoke(boost::json::value_to_tag<GetUsagePlanCounterResponse>, boost::json::value const &v) {
            GetUsagePlanCounterResponse r;
            if (Core::Json::AttributeExists(v, "usagePlan")) {
                r.usagePlan = boost::json::value_to<UsagePlan>(v.at("usagePlan"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetUsagePlanCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"usagePlan", boost::json::value_from(obj.usagePlan)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
