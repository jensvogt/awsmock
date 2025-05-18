//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_MONITORING_GET_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_MONITORING_GET_COUNTERS_RESPONSE_H

// C++ standard includes
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/monitoring/Counter.h>

namespace AwsMock::Dto::Monitoring {

    /**
     * @brief Get counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetCountersResponse final : Common::BaseCounter<GetCountersResponse> {

        /**
         * Counters
         */
        std::vector<Counter> counters;

      private:

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetCountersResponse const &obj) {
            boost::json::array countersJson;
            for (const auto &c: obj.counters) {
                boost::json::array v;
                v.emplace_back(Core::DateTimeUtils::ToISO8601(c.timestamp));
                v.emplace_back(c.performanceValue);
                countersJson.emplace_back(v);
            }
            jv = {{"counters", countersJson}};
        }
    };

}// namespace AwsMock::Dto::Monitoring

#endif// AWSMOCK_DTO_MONITORING_GET_COUNTERS_RESPONSE_H
