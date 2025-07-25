//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DELETE_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DELETE_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Delete group request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteGroupRequest final : Common::BaseCounter<DeleteGroupRequest> {

        /**
         * Group name
         */
        std::string groupName;

        /**
         * User pool userPoolId
         */
        std::string userPoolId;

      private:

        friend DeleteGroupRequest tag_invoke(boost::json::value_to_tag<DeleteGroupRequest>, boost::json::value const &v) {
            DeleteGroupRequest r;
            r.groupName = Core::Json::GetStringValue(v, "GroupName");
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteGroupRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"GroupName", obj.groupName},
                    {"UserPoolId", obj.userPoolId},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DELETE_GROUP_REQUEST_H
