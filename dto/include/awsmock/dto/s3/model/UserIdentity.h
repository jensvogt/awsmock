//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::S3 {

    /**
     * @brief S3 object user identity DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserIdentity final : Common::BaseObject<UserIdentity> {

        /**
         * AWS principal ID
         */
        std::string principalId;

      private:

        friend UserIdentity tag_invoke(boost::json::value_to_tag<UserIdentity>, boost::json::value const &v) {
            UserIdentity r;
            r.principalId = Core::Json::GetStringValue(v, "principalId");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserIdentity const &obj) {
            jv = {
                    {"principalId", obj.principalId},
            };
        }
    };

}// namespace Awsmock::Dto::S3
