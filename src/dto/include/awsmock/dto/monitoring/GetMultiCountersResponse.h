//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_MONITORING_GET_MULTI_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_MONITORING_GET_MULTI_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/monitoring/Counter.h>

namespace AwsMock::Dto::Monitoring {

    /**
     * @brief Get counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetMultiCountersResponse final : Common::BaseCounter<GetMultiCountersResponse> {

        /**
         * Counters
         */
        std::map<std::string, std::vector<Counter>> counters;

      private:

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetMultiCountersResponse const &obj) {

            jv = {};
            boost::json::object rootObject;
            for (const auto &[fst, snd]: obj.counters) {
                boost::json::array countersJson;
                for (const auto &c: snd) {
                    boost::json::array v;
                    v.emplace_back(Core::DateTimeUtils::ToISO8601(c.timestamp));
                    v.emplace_back(c.performanceValue);
                    countersJson.emplace_back(v);
                }
                jv.as_object()[fst] = countersJson;
            }
        }
    };

}// namespace AwsMock::Dto::Monitoring

#endif// AWSMOCK_DTO_MONITORING_GET_COUNTERS_RESPONSE_H
