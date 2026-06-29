//
// Created by vogje01 on 4/30/24.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::Lambda {

    struct InstanceCounter final : Common::BaseObject<InstanceCounter> {

        /**
         * @brief Name of the lambda function
         */
        std::string lambdaName;

        /**
         * @brief Instance ID
         */
        std::string instanceId;

        /**
         * @brief Container ID
         */
        std::string containerId;

        /**
         * @brief Status
         */
        std::string status;

        /**
         * @brief Invocation duration in milliseconds
         */
        long duration{};

        /**
         * @brief Container hostname
         */
        std::string hostname;

        /**
         * @brief Container private port
         */
        long privatePort{};

        /**
         * @brief Container public port
         */
        long publicPort{};

        /**
         * @brief Last started
         */
        system_clock::time_point lastStart;

        /**
         * @brief Last invocation
         */
        system_clock::time_point lastInvocation;

        /**
         * @brief Last started
         */
        system_clock::time_point lastStop;

      private:

        friend InstanceCounter tag_invoke(boost::json::value_to_tag<InstanceCounter>, boost::json::value const &v) {
            InstanceCounter r;
            r.lambdaName = Core::Json::GetStringValue(v, "lambdaName");
            r.instanceId = Core::Json::GetStringValue(v, "instanceId");
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            r.status = Core::Json::GetStringValue(v, "status");
            r.duration = Core::Json::GetLongValue(v, "duration");
            r.hostname = Core::Json::GetStringValue(v, "hostname");
            r.privatePort = Core::Json::GetLongValue(v, "privatePort");
            r.publicPort = Core::Json::GetLongValue(v, "publicPort");
            r.lastStart = Core::Json::GetDatetimeValue(v, "lastStart");
            r.lastInvocation = Core::Json::GetDatetimeValue(v, "lastInvocation");
            r.lastStop = Core::Json::GetDatetimeValue(v, "lastStop");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, InstanceCounter const &obj) {
            jv = {
                    {"lambdaName", obj.lambdaName},
                    {"instanceId", obj.instanceId},
                    {"containerId", obj.containerId},
                    {"status", obj.status},
                    {"duration", obj.duration},
                    {"hostname", obj.hostname},
                    {"privatePort", obj.privatePort},
                    {"publicPort", obj.publicPort},
                    {"lastStart", Core::DateTimeUtils::ToISO8601(obj.lastStart)},
                    {"lastInvocation", Core::DateTimeUtils::ToISO8601(obj.lastInvocation)},
                    {"lastStop", Core::DateTimeUtils::ToISO8601(obj.lastStop)},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
