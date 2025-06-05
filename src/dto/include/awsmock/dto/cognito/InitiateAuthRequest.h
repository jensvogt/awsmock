//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_INITIATE_AUTH_REQUEST_H
#define AWSMOCK_DTO_COGNITO_INITIATE_AUTH_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/AuthFlow.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Initiate authentication request
     *
     * Example:
     * @code{.json}
     * {
     *   "AnalyticsMetadata": {
     *      "AnalyticsEndpointId": "string"
     *   },
     *   "AuthFlow": "string",
     *   "AuthParameters": {
     *      "string" : "string"
     *   },
     *   "ClientId": "string",
     *   "ClientMetadata": {
     *      "string" : "string"
     *   },
     *   "UserContextData": {
     *      "EncodedData": "string",
     *      "IpAddress": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct InitiateAuthRequest final : Common::BaseCounter<InitiateAuthRequest> {

        /**
         * Auth flow
         *
         * Valid Values: USER_SRP_AUTH | REFRESH_TOKEN_AUTH | REFRESH_TOKEN | CUSTOM_AUTH | ADMIN_NO_SRP_AUTH | USER_PASSWORD_AUTH | ADMIN_USER_PASSWORD_AUTH
         */
        AuthFlowType authFlow;

        /**
         * Client ID
         */
        std::string clientId;

        /**
         * Auth parameters
         */
        std::map<std::string, std::string> authParameters;

        /**
         * Client metadata
         */
        std::map<std::string, std::string> clientMetaData;

        /**
         * @brief Returns the username
         *
         * @return user ID
         */
        std::string GetUserId() {
            return authParameters["USERNAME"];
        }

        /**
         * @brief Returns the user password
         *
         * @return user password
         */
        std::string GetPassword() {
            return authParameters["PASSWORD"];
        }

        /**
         * @brief Returns the client secret from the AuthParameters
         *
         * @return client secret
         */
        std::string GetClientSecret() {
            return authParameters["SECRET_HASH"];
        }

      private:

        friend InitiateAuthRequest tag_invoke(boost::json::value_to_tag<InitiateAuthRequest>, boost::json::value const &v) {
            InitiateAuthRequest r;
            r.authFlow = AuthFlowTypeFromString(Core::Json::GetStringValue(v, "AuthFlow"));
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            r.authParameters = boost::json::value_to<std::map<std::string, std::string>>(v.at("AuthParameters"));
            r.clientMetaData = boost::json::value_to<std::map<std::string, std::string>>(v.at("ClientMetaData"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, InitiateAuthRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"AuthFlow", AuthFlowTypeToString(obj.authFlow)},
                    {"ClientId", obj.clientId},
                    {"AuthParameters", boost::json::value_from(obj.clientId)},
                    {"ClientMetaData", boost::json::value_from(obj.clientMetaData)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_INITIATE_AUTH_REQUEST_H
