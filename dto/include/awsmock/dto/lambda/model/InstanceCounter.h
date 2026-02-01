//
// Created by vogje01 on 4/30/24.
//

#ifndef AWSMOCK_DTO_LAMBDA_INSTANCE_COUNTER_H
#define AWSMOCK_DTO_LAMBDA_INSTANCE_COUNTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct InstanceCounter final : Common::BaseCounter<InstanceCounter> {

        /**
         * Instance ID
         */
        std::string instanceId;

        /**
         * Container ID
         */
        std::string containerId;

        /**
         * Status
         */
        std::string status;

        /**
         * Invocation duration in milliseconds
         */
        long duration{};

        /**
         * Container hostname
         */
        std::string hostname;

        /**
         * Container private port
         */
        long privatePort{};

        /**
         * Container public port
         */
        long publicPort{};

        /**
         * Last started
         */
        system_clock::time_point lastInvocation;

      private:

        friend InstanceCounter tag_invoke(boost::json::value_to_tag<InstanceCounter>, boost::json::value const &v) {
            InstanceCounter r;
            r.instanceId = Core::Json::GetStringValue(v, "instanceId");
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            r.status = Core::Json::GetStringValue(v, "status");
            r.duration = Core::Json::GetLongValue(v, "duration");
            r.hostname = Core::Json::GetStringValue(v, "hostname");
            r.privatePort = Core::Json::GetLongValue(v, "privatePort");
            r.publicPort = Core::Json::GetLongValue(v, "publicPort");
            r.lastInvocation = Core::Json::GetDatetimeValue(v, "lastInvocation");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, InstanceCounter const &obj) {
            jv = {
                    {"instanceId", obj.instanceId},
                    {"containerId", obj.containerId},
                    {"status", obj.status},
                    {"duration", obj.duration},
                    {"hostname", obj.hostname},
                    {"privatePort", obj.privatePort},
                    {"publicPort", obj.publicPort},
                    {"lastInvocation", Core::DateTimeUtils::ToISO8601(obj.lastInvocation)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_INSTANCE_COUNTER_H
