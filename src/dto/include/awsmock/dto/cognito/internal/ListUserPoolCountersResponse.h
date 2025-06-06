//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/UserPoolCounter.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolCountersResponse final : Common::BaseCounter<ListUserPoolCountersResponse> {

        /**
         * User pool entities
         */
        std::vector<UserPoolCounter> userPoolCounters;

        /**
         * Total number of user pools
         */
        long total{};

      private:

        friend ListUserPoolCountersResponse tag_invoke(boost::json::value_to_tag<ListUserPoolCountersResponse>, boost::json::value const &v) {
            ListUserPoolCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "userPoolCounters")) {
                r.userPoolCounters = boost::json::value_to<std::vector<UserPoolCounter>>(v.at("userPoolCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserPoolCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"userPoolCounters", boost::json::value_from(obj.userPoolCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_COUNTERS_RESPONSE_H
