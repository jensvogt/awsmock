//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_COGNITO_VALIDATION_DATA_H
#define AWSMOCK_DTO_COGNITO_VALIDATION_DATA_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/entity/cognito/User.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief User attribute
     *
     * @par
     * Temporary user attributes that contribute to the outcomes of your pre sign-up Lambda trigger. This set of key-value pairs are for custom validation of information that you collect from your
     * users but don't need to retain. Your Lambda function can analyze this additional data and act on it. Your function might perform external API operations like logging user attributes and
     * validation data to Amazon CloudWatch Logs. Validation data might also affect the response that your function returns to Amazon Cognito, like automatically confirming the user if they sign
     * up from within your network.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ValidationData final : Common::BaseCounter<ValidationData> {

        /**
         * User attribute name
         */
        std::string name;

        /**
         * MessageAttribute value
         */
        std::string attributeValue;

        /**
         * @brief Converts the JSON string to a DTO
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                name = Core::Bson::BsonUtils::GetStringValue(document, "Name");
                attributeValue = Core::Bson::BsonUtils::GetStringValue(document, "Value");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Name", name);
                Core::Bson::BsonUtils::SetStringValue(document, "Value", attributeValue);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend ValidationData tag_invoke(boost::json::value_to_tag<ValidationData>, boost::json::value const &v) {
            ValidationData r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.attributeValue = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ValidationData const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"Value", obj.attributeValue},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_VALIDATION_DATA_H
