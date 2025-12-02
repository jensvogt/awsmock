//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_LIST_PARAMETER_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SSM_LIST_PARAMETER_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/ssm/model/Parameter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief List all parameters response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListParameterCountersResponse final : Common::BaseCounter<ListParameterCountersResponse> {

        /**
         * List of queues ARNs
         */
        std::vector<Parameter> parameterCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListParameterCountersResponse tag_invoke(boost::json::value_to_tag<ListParameterCountersResponse>, boost::json::value const &v) {
            ListParameterCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "parameterCounters")) {
                r.parameterCounters = boost::json::value_to<std::vector<Parameter>>(v.at("parameterCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListParameterCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"parameterCounters", boost::json::value_from(obj.parameterCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_LIST_PARAMETER_COUNTERS_RESPONSE_H
