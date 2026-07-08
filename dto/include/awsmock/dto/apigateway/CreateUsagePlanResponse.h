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
     * @brief Create API gateway usage plan response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateUsagePlanResponse final : Common::BaseCounter<CreateUsagePlanResponse> {

        /**
         * Usage plan ID
         */
        std::string id;

        /**
         * Usage plan name
         */
        std::string name;

        /**
         * Description
         */
        std::string description;

        /**
         * Quota limit
         */
        long quotaLimit{};

        /**
         * Quota offset
         */
        long quotaOffset{};

        /**
         * Quota period
         */
        std::string quotaPeriod;

        /**
         * Throttle burst limit
         */
        long throttleBurstLimit{};

        /**
         * Throttle rate limit
         */
        double throttleRateLimit{};

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend CreateUsagePlanResponse tag_invoke(boost::json::value_to_tag<CreateUsagePlanResponse>, boost::json::value const &v) {
            CreateUsagePlanResponse r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.name = Core::Json::GetStringValue(v, "name");
            r.description = Core::Json::GetStringValue(v, "description");
            if (Core::Json::AttributeExists(v, "quota")) {
                const auto &q = v.at("quota");
                r.quotaLimit = Core::Json::GetLongValue(q, "limit");
                r.quotaOffset = Core::Json::GetLongValue(q, "offset");
                r.quotaPeriod = Core::Json::GetStringValue(q, "period");
            }
            if (Core::Json::AttributeExists(v, "throttle")) {
                const auto &t = v.at("throttle");
                r.throttleBurstLimit = Core::Json::GetLongValue(t, "burstLimit");
                r.throttleRateLimit = Core::Json::GetDoubleValue(t, "rateLimit");
            }
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUsagePlanResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"id", obj.id},
                    {"name", obj.name},
                    {"description", obj.description},
                    {"quota", {{"limit", obj.quotaLimit}, {"offset", obj.quotaOffset}, {"period", obj.quotaPeriod}}},
                    {"throttle", {{"burstLimit", obj.throttleBurstLimit}, {"rateLimit", obj.throttleRateLimit}}},
                    {"tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
