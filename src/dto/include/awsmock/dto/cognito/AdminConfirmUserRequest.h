//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CONFIRM_USER_REQUEST_H
#define AWSMOCK_DTO_COGNITO_CONFIRM_USER_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Confirm user request
     *
     * Example:
     * @code{.json}
     * {
     *   "AnalyticsMetadata": {
     *      "AnalyticsEndpointId": "string"
     *   },
     *   "ClientId": "string",
     *   "ClientMetadata": {
     *      "string" : "string"
     *   },
     *   "ConfirmationCode": "string",
     *   "ForceAliasCreation": boolean,
     *   "SecretHash": "string",
     *   "UserContextData": {
     *      "EncodedData": "string",
     *      "IpAddress": "string"
     *   },
     *   "Username": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminConfirmUserRequest : Common::BaseRequest<AdminConfirmUserRequest> {

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Client metadata
         */
        std::map<std::string, std::string> clientMetadata;

        /**
         * Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CONFIRM_USER_REQUEST_H
