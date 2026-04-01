//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_AUTHENTICATION_RESULT_H
#define AWSMOCK_DTO_COGNITO_MODEL_AUTHENTICATION_RESULT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito authentication result
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AuthenticationResult final : Common::BaseCounter<AuthenticationResult> {

        /**
         * Access token
         */
        std::string accessToken;

        /**
         * ID token
         */
        std::string idToken;

        /**
         * Refresh token
         */
        std::string refreshToken;

        /**
         * Token type
         */
        std::string tokenType = "Bearer";

        /**
         * Expires in
         */
        long expiredIn{};

      private:

        friend AuthenticationResult tag_invoke(boost::json::value_to_tag<AuthenticationResult>, boost::json::value const &v) {
            AuthenticationResult r;
            r.accessToken = Core::Json::GetStringValue(v, "accessToken");
            r.idToken = Core::Json::GetStringValue(v, "idToken");
            r.refreshToken = Core::Json::GetStringValue(v, "refreshToken");
            r.tokenType = Core::Json::GetStringValue(v, "tokenType");
            r.expiredIn = Core::Json::GetLongValue(v, "expiredIn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AuthenticationResult const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"accessToken", obj.accessToken},
                    {"idToken", obj.idToken},
                    {"refreshToken", obj.refreshToken},
                    {"tokenType", obj.tokenType},
                    {"expiredIn", obj.expiredIn},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_AUTHENTICATION_RESULT_H
