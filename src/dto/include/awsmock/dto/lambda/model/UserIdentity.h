//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_USER_IDENTITY_H
#define AWSMOCK_DTO_LAMBDA_USER_IDENTITY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Lambda user identity
     *
     * @author jens.vogt\@opitz--consulting.com
     */
    struct UserIdentity final : Common::BaseCounter<UserIdentity> {

        /**
         * AWS principal ID
         */
        std::string principalId;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "principalId", principalId);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend UserIdentity tag_invoke(boost::json::value_to_tag<UserIdentity>, boost::json::value const &v) {
            UserIdentity r;
            r.principalId = Core::Json::GetStringValue(v, "PrincipalId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserIdentity const &obj) {
            jv = {
                    {"PrincipalId", obj.principalId},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_USER_IDENTITY_H
