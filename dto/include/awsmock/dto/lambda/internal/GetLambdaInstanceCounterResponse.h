//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/InstanceCounter.h>

namespace Awsmock::Dto::Lambda {

    /**
     * @brief Get function instance counters response
     */
    struct GetLambdaInstanceCounterResponse final : Common::BaseCounter<GetLambdaInstanceCounterResponse> {

        /**
         * List of instance counters
         */
        InstanceCounter instanceCounter;

      private:

        friend GetLambdaInstanceCounterResponse tag_invoke(boost::json::value_to_tag<GetLambdaInstanceCounterResponse>, boost::json::value const &v) {
            GetLambdaInstanceCounterResponse r;
            r.instanceCounter = boost::json::value_to<InstanceCounter>(v.at("instanceCounter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetLambdaInstanceCounterResponse const &obj) {
            jv = {
                    {"instanceCounter", boost::json::value_from(obj.instanceCounter)},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
