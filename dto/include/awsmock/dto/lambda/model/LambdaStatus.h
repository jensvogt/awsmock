//
// Created by vogje01 on 6/20/26.
//

#pragma once

// Awsmock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/dto/common/BaseObject.h>
#include <awsmock/dto/lambda/model/LambdaRuntimeStatus.h>

namespace Awsmock::Dto::Lambda {

    struct LambdaStatus : Common::BaseObject<LambdaStatus> {

        /**
         * @brief Runtime status
         */
        LambdaRuntimeStatus runtimeStatus = LambdaRuntimeStatus::stopped;

        /**
         * @brief PID
         */
        int pid{};

        /**
         * @brief Invocation count
         */
        int invocations{};

        /**
         * @brief Average invocation duration in milliseconds
         */
        double avgDuration{};

        /**
         * @brief Lambda function name — set by the GRT before reporting to awsmockmgr
         */
        std::string functionName;

        /**
         * @brief HTTP port the GRT is listening to on — identifies the instance within a function
         */
        int port{};

        /**
         * @brief Unique ID of the lambda runtime instance
         */
        std::string instanceId;

        /**
         * @brief Last start timestamp
         */
        system_clock::time_point lastStart;

        /**
         * @brief Last invocation timestamp
         */
        system_clock::time_point lastInvocation;

        /**
         * @brief Last stop timestamp
         */
        system_clock::time_point lastStop;

      private:

        friend LambdaStatus tag_invoke(boost::json::value_to_tag<LambdaStatus>, boost::json::value const &v) {
            LambdaStatus r;
            r.runtimeStatus = runtimeStatusFromString(Core::Json::GetStringValue(v, "runtimeStatus"));
            r.pid = Core::Json::GetIntValue(v, "pid");
            r.instanceId = Core::Json::GetStringValue(v, "instanceId");
            r.invocations = Core::Json::GetIntValue(v, "invocations");
            r.avgDuration = Core::Json::GetDoubleValue(v, "avgDuration");
            r.functionName = Core::Json::GetStringValue(v, "functionName");
            r.lastStart = Core::Json::GetDatetimeValue(v, "lastStart");
            r.lastInvocation = Core::Json::GetDatetimeValue(v, "lastInvocation");
            r.lastStop = Core::Json::GetDatetimeValue(v, "lastStop");
            r.port = Core::Json::GetIntValue(v, "port");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LambdaStatus const &obj) {
            jv = {
                    {"runtimeStatus", runtimeStatusToString(obj.runtimeStatus)},
                    {"pid", obj.pid},
                    {"instanceId", obj.instanceId},
                    {"invocations", obj.invocations},
                    {"avgDuration", obj.avgDuration},
                    {"functionName", obj.functionName},
                    {"lastStart", Core::DateTimeUtils::ToISO8601(obj.lastStart)},
                    {"lastInvocation", Core::DateTimeUtils::ToISO8601(obj.lastInvocation)},
                    {"lastStop", Core::DateTimeUtils::ToISO8601(obj.lastStop)},
                    {"port", obj.port},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
