//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_COGNITO_USER_ATTRIBUTE_H
#define AWSMOCK_DTO_COGNITO_USER_ATTRIBUTE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/entity/cognito/User.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief User attribute
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserAttribute final : Common::BaseCounter<UserAttribute> {

        /**
         * User attribute name
         */
        std::string name;

        /**
         * MessageAttribute value
         */
        std::string attributeValue;

        /**
         * @brief Converts the BSON string to a DTO
         *
         * @param document BSON object
         */
        void FromDocument(const std::optional<view> &document) {

            try {

                name = Core::Bson::BsonUtils::GetStringValue(document, "Name");
                attributeValue = Core::Bson::BsonUtils::GetStringValue(document, "Value");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend UserAttribute tag_invoke(boost::json::value_to_tag<UserAttribute>, boost::json::value const &v) {
            UserAttribute r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.attributeValue = Core::Json::GetStringValue(v, "attributeValue");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserAttribute const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"attributeValue", obj.attributeValue},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_USER_ATTRIBUTE_H
