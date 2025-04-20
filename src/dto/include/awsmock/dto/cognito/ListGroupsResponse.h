//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_GROUPS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_GROUPS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/Group.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List groups response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListGroupsResponse final : Common::BaseRequest<ListGroupsResponse> {

        /**
         * Group entities
         */
        std::vector<Group> groups{};

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_GROUPS_RESPONSE_H
