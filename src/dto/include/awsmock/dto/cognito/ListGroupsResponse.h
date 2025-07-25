//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_GROUPS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_GROUPS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/cognito/model/Group.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List groups response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListGroupsResponse final : Common::BaseCounter<ListGroupsResponse> {

        /**
         * Group entities
         */
        std::vector<Group> groups{};

      private:

        friend ListGroupsResponse tag_invoke(boost::json::value_to_tag<ListGroupsResponse>, boost::json::value const &v) {
            ListGroupsResponse r;
            r.groups = boost::json::value_to<std::vector<Group>>(v.at("Groups"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListGroupsResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Groups", boost::json::value_from(obj.groups)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_GROUPS_RESPONSE_H
