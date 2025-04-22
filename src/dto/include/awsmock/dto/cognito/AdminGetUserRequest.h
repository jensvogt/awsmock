//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Admin get user request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminGetUserRequest final : Common::BaseDto<AdminGetUserRequest> {

        /**
         * ID of the user pool
         */
        std::string userPoolId;

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * @brief Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_REQUEST_H
