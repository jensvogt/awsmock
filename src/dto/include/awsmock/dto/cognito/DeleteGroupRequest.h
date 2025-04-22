//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DELETE_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DELETE_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Delete group request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteGroupRequest final : Common::BaseDto<DeleteGroupRequest> {

        /**
         * Group name
         */
        std::string groupName;

        /**
         * User pool userPoolId
         */
        std::string userPoolId;

        /**
         * Convert from a JSON object.
         *
         * @param jsonString json string object
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DELETE_GROUP_REQUEST_H
