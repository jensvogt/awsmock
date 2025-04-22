//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/UserPool.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolResponse : Common::BaseDto<ListUserPoolResponse> {

        /**
         * User pool entities
         */
        std::vector<UserPool> userPools;

        /**
         * Total number of user pools
         */
        long total;

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H
