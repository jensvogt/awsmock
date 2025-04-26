//
// Created by vogje01 on 4/20/24.
//

#ifndef AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H
#define AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

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
    struct CustomEmailSender final : Common::BaseDto<CustomEmailSender> {

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CUSTOM_EMAIL_SENDER_H
