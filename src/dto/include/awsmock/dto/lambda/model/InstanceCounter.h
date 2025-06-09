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

      private:

        friend InstanceCounter tag_invoke(boost::json::value_to_tag<InstanceCounter>, boost::json::value const &v) {
            InstanceCounter r;
            r.instanceId = Core::Json::GetStringValue(v, "instanceId");
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            r.status = Core::Json::GetStringValue(v, "status");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, InstanceCounter const &obj) {
            jv = {
                    {"instanceId", obj.instanceId},
                    {"containerId", obj.containerId},
                    {"status", obj.status},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_INSTANCE_COUNTER_H
