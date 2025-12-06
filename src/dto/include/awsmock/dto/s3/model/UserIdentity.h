//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_USER_IDENTITY_H
#define AWSMOCK_DTO_S3_USER_IDENTITY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 object user identity DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserIdentity final : Common::BaseCounter<UserIdentity> {

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
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"principalId", obj.principalId},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_USER_IDENTITY_H
