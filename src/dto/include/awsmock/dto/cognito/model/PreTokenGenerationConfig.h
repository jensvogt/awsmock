//
// Created by vogje01 on 4/20/24.
//

#ifndef AWSMOCK_DTO_COGNITO_PRE_GENERATION_CONFIG_H
#define AWSMOCK_DTO_COGNITO_PRE_GENERATION_CONFIG_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Cognito pre generation config
     *
     * Example:
     * @code{.json}
     * "PreTokenGenerationConfig": {
     *    "LambdaArn": "string",
     *    "LambdaVersion": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PreTokenGenerationConfig {

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * Lambda version
         */
        std::string lambdaVersion;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const PreTokenGenerationConfig &r);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_PRE_GENERATION_CONFIG_H
