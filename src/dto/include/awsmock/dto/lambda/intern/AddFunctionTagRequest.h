//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_TAG_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_TAG_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Add function tag request
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionArn": "string" ,
     *   "Key": string,
     *   "Value": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AddFunctionTagRequest {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Tag key
         */
        std::string tagKey;

        /**
         * Tag value
         */
        std::string tagValue;

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Parse a JSON string.
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const AddFunctionTagRequest &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_TAG_REQUEST_H
