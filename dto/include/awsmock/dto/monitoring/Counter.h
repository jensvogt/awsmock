//
// Created by vogje01 on 10/2/24.
//

#ifndef AWSMOCK_DTO_MONITORING_COUNTER_H
#define AWSMOCK_DTO_MONITORING_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Monitoring {

    using std::chrono::system_clock;

    /**
     * @brief Monitoring counter
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Counter final : Common::BaseCounter<Counter> {

        /**
         * Value
         */
        double performanceValue{};

        /**
         * Timestamp
         */
        system_clock::time_point timestamp = system_clock::now();

      private:

        friend Counter tag_invoke(boost::json::value_to_tag<Counter>, boost::json::value const &v) {
            Counter r;
            r.performanceValue = Core::Json::GetDoubleValue(v, "value");
            r.timestamp = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "timestamp"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Counter const &obj) {
            jv = {
                    {"value", obj.performanceValue},
                    {"timestamp", Core::DateTimeUtils::ToISO8601(obj.timestamp)},
            };
        }
    };

};// namespace AwsMock::Dto::Monitoring

#endif// AWSMOCK_DTO_MONITORING_COUNTER_H
