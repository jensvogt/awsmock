//
// Created by vogje01 on 07/14/2026
//

#pragma once

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::ApiGateway {

    struct UsagePlanQuota {
        long limit{};
        long offset{};
        std::string period;

      private:

        friend UsagePlanQuota tag_invoke(boost::json::value_to_tag<UsagePlanQuota>, boost::json::value const &v) {
            UsagePlanQuota r;
            r.limit = Core::Json::GetLongValue(v, "limit");
            r.offset = Core::Json::GetLongValue(v, "offset");
            r.period = Core::Json::GetStringValue(v, "period");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UsagePlanQuota const &obj) {
            jv = {
                    {"limit", obj.limit},
                    {"offset", obj.offset},
                    {"period", obj.period},
            };
        }
    };

    struct UsagePlanThrottle {
        long burstLimit{};
        double rateLimit{};

      private:

        friend UsagePlanThrottle tag_invoke(boost::json::value_to_tag<UsagePlanThrottle>, boost::json::value const &v) {
            UsagePlanThrottle r;
            r.burstLimit = Core::Json::GetLongValue(v, "burstLimit");
            r.rateLimit = Core::Json::GetDoubleValue(v, "rateLimit");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UsagePlanThrottle const &obj) {
            jv = {
                    {"burstLimit", obj.burstLimit},
                    {"rateLimit", obj.rateLimit},
            };
        }
    };

    struct UsagePlan final : Common::BaseObject<UsagePlan> {

        std::string id;

        std::string name;

        std::string description;

        UsagePlanQuota quota;

        UsagePlanThrottle throttle;

        std::map<std::string, std::string> tags;

        system_clock::time_point created;

        system_clock::time_point modified;

      private:

        friend UsagePlan tag_invoke(boost::json::value_to_tag<UsagePlan>, boost::json::value const &v) {
            UsagePlan r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.name = Core::Json::GetStringValue(v, "name");
            r.description = Core::Json::GetStringValue(v, "description");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            if (Core::Json::AttributeExists(v, "quota")) {
                r.quota = boost::json::value_to<UsagePlanQuota>(v.at("quota"));
            }
            if (Core::Json::AttributeExists(v, "throttle")) {
                r.throttle = boost::json::value_to<UsagePlanThrottle>(v.at("throttle"));
            }
            if (Core::Json::AttributeExists(v, "tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UsagePlan const &obj) {
            jv = {
                    {"id", obj.id},
                    {"name", obj.name},
                    {"description", obj.description},
                    {"quota", boost::json::value_from(obj.quota)},
                    {"throttle", boost::json::value_from(obj.throttle)},
                    {"tags", boost::json::value_from(obj.tags)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace Awsmock::Dto::ApiGateway
