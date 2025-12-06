//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Delete user pool request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteUserPoolRequest final : Common::BaseCounter<DeleteUserPoolRequest> {

        /**
         * User pool userPoolId
         */
        std::string userPoolId;

      private:

        friend DeleteUserPoolRequest tag_invoke(boost::json::value_to_tag<DeleteUserPoolRequest>, boost::json::value const &v) {
            DeleteUserPoolRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteUserPoolRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DELETE_USERPOOL_REQUEST_H
