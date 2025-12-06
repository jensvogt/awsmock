//
// Created by vogje01 on 8/4/24.
//

#ifndef AWSMOCK_DTO_COGNITO_SIGN_UP_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_SIGN_UP_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Sign up response
     *
     * @code
     * {
     *   "CodeDeliveryDetails": {
     *      "AttributeName": "string",
     *      "DeliveryMedium": "string",
     *      "Destination": "string"
     *   },
     *   "UserConfirmed": boolean,
     *   "UserSub": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SignUpResponse final : Common::BaseCounter<SignUpResponse> {

        /**
         * User substitution
         *
         * The 128-bit ID of the authenticated user. This isn't the same as username.
         */
        std::string userSub;

        /**
         * User confirmed
         *
         * A response from the server indicating that a user registration has been confirmed.
         */
        bool userConfirmed = false;

      private:

        friend SignUpResponse tag_invoke(boost::json::value_to_tag<SignUpResponse>, boost::json::value const &v) {
            SignUpResponse r;
            r.userSub = Core::Json::GetStringValue(v, "UserSub");
            r.userConfirmed = Core::Json::GetBoolValue(v, "UserConfirmed");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SignUpResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserSub", obj.userSub},
                    {"UserConfirmed", obj.userConfirmed},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_SIGN_UP_RESPONSE_H
