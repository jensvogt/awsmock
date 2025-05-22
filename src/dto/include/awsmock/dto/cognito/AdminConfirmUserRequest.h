//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CONFIRM_USER_REQUEST_H
#define AWSMOCK_DTO_COGNITO_CONFIRM_USER_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

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
    struct AdminConfirmUserRequest final : Common::BaseCounter<AdminConfirmUserRequest> {

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

      private:

        friend AdminConfirmUserRequest tag_invoke(boost::json::value_to_tag<AdminConfirmUserRequest>, boost::json::value const &v) {
            AdminConfirmUserRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.userName = Core::Json::GetStringValue(v, "userName");
            if (Core::Json::AttributeExists(v, "clientMetadata")) {
                r.clientMetadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("attributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminConfirmUserRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userPoolId", obj.userPoolId},
                    {"userName", obj.userName},
                    {"clientMetadata", boost::json::value_from(obj.clientMetadata)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CONFIRM_USER_REQUEST_H
