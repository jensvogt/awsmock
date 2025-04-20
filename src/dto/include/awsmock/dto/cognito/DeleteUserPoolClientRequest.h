//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_CLIENT_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_CLIENT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Delete user pool client request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteUserPoolClientRequest : Common::BaseRequest<DeleteUserPoolClientRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User pool client ID
         */
        std::string clientId;

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
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_CLIENT_REQUEST_H
