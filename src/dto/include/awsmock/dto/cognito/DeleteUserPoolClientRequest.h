//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_CLIENT_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_CLIENT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Delete user pool client request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteUserPoolClientRequest final : Common::BaseCounter<DeleteUserPoolClientRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User pool client ID
         */
        std::string clientId;

      private:

        friend DeleteUserPoolClientRequest tag_invoke(boost::json::value_to_tag<DeleteUserPoolClientRequest>, boost::json::value const &v) {
            DeleteUserPoolClientRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteUserPoolClientRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"ClientId", obj.clientId},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_CLIENT_REQUEST_H
