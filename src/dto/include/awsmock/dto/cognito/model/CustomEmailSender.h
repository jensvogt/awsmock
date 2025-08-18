//
// Created by vogje01 on 4/20/24.
//

#ifndef AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H
#define AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * Cognito custom email sender
     *
     * Example:
     * @code{.json}
     * "CustomEmailSender": {
     *    "LambdaArn": "string",
     *    "LambdaVersion": "string"
     * },
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CustomEmailSender final : Common::BaseCounter<CustomEmailSender> {

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * Lambda version
         */
        std::string lambdaVersion;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "lambdaArn", lambdaArn);
                Core::Bson::BsonUtils::SetStringValue(document, "lambdaVersion", lambdaVersion);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend CustomEmailSender tag_invoke(boost::json::value_to_tag<CustomEmailSender>, boost::json::value const &v) {
            CustomEmailSender r;
            r.lambdaArn = Core::Json::GetStringValue(v, "LambdaArn");
            r.lambdaVersion = Core::Json::GetStringValue(v, "LambdaVersion");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CustomEmailSender const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"LambdaArn", obj.lambdaArn},
                    {"LambdaVersion", obj.lambdaVersion},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H
