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
     * @brief List usage plan counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsagePlanCountersResponse final : Common::BaseCounter<ListUsagePlanCountersResponse> {

        /**
         * Usage plans
         */
        std::vector<UsagePlan> usagePlans;

        /**
         * Total number of usage plans
         */
        long total{};

      private:

        friend ListUsagePlanCountersResponse tag_invoke(boost::json::value_to_tag<ListUsagePlanCountersResponse>, boost::json::value const &v) {
            ListUsagePlanCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "usagePlans")) {
                r.usagePlans = boost::json::value_to<std::vector<UsagePlan>>(v.at("usagePlans"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUsagePlanCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"usagePlans", boost::json::value_from(obj.usagePlans)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
