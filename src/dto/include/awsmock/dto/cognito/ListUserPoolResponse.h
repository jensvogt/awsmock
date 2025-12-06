//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/UserPool.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolResponse final : Common::BaseCounter<ListUserPoolResponse> {

        /**
         * User pool entities
         */
        std::vector<UserPool> userPools;

        /**
         * Total number of user pools
         */
        long total{};

      private:

        friend ListUserPoolResponse tag_invoke(boost::json::value_to_tag<ListUserPoolResponse>, boost::json::value const &v) {
            ListUserPoolResponse r;
            r.userPools = boost::json::value_to<std::vector<UserPool>>(v.at("UserPools"));
            r.total = Core::Json::GetLongValue(v, "Total");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserPoolResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPools", boost::json::value_from(obj.userPools)},
                    {"Total", obj.total},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H
