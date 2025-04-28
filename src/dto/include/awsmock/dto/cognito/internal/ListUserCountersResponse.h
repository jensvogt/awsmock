//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/UserCounter.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserCountersResponse final : Common::BaseCounter<ListUserCountersResponse> {

        /**
         * User entities
         */
        std::vector<UserCounter> users;

        /**
         * User entities
         */
        long total{};

      private:

        friend ListUserCountersResponse tag_invoke(boost::json::value_to_tag<ListUserCountersResponse>, boost::json::value const &v) {
            ListUserCountersResponse r;
            r.region = v.at("region").as_string();
            r.requestId = v.at("requestId").as_string();
            r.user = v.at("user").as_string();
            r.users = boost::json::value_to<std::vector<UserCounter>>(v.at("users"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"users", boost::json::value_from(obj.users)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_RESPONSE_H
