//
// Created by vogje01 on 07/08/2026
//

#pragma once

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::ApiGateway {

    /**
     * @brief Create usage plan quota
     */
    struct CreateUsagePlanQuota {
        long limit{};
        long offset{};
        std::string period;
    };

    /**
     * @brief Create usage plan throttle
     */
    struct CreateUsagePlanThrottle {
        long burstLimit{};
        double rateLimit{};
    };

    /**
     * @brief Create API gateway usage plan request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateUsagePlanRequest final : Common::BaseCounter<CreateUsagePlanRequest> {

        /**
         * Usage plan name
         */
        std::string name;

        /**
         * Description
         */
        std::string description;

        /**
         * Quota settings
         */
        CreateUsagePlanQuota quota;

        /**
         * Throttle settings
         */
        CreateUsagePlanThrottle throttle;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend CreateUsagePlanRequest tag_invoke(boost::json::value_to_tag<CreateUsagePlanRequest>, boost::json::value const &v) {
            CreateUsagePlanRequest r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.description = Core::Json::GetStringValue(v, "description");
            if (Core::Json::AttributeExists(v, "quota")) {
                const auto &q = v.at("quota");
                r.quota.limit = Core::Json::GetLongValue(q, "limit");
                r.quota.offset = Core::Json::GetLongValue(q, "offset");
                r.quota.period = Core::Json::GetStringValue(q, "period");
            }
            if (Core::Json::AttributeExists(v, "throttle")) {
                const auto &t = v.at("throttle");
                r.throttle.burstLimit = Core::Json::GetLongValue(t, "burstLimit");
                r.throttle.rateLimit = Core::Json::GetDoubleValue(t, "rateLimit");
            }
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUsagePlanRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"name", obj.name},
                    {"description", obj.description},
                    {"quota", {{"limit", obj.quota.limit}, {"offset", obj.quota.offset}, {"period", obj.quota.period}}},
                    {"throttle", {{"burstLimit", obj.throttle.burstLimit}, {"rateLimit", obj.throttle.rateLimit}}},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
