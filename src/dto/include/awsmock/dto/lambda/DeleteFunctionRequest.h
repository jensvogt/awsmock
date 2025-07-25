//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Delete function request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteFunctionRequest {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Name of the function
         */
        std::string functionName;

        /**
         * Qualifier
         */
        std::string qualifier;

        /**
         * @brief Parse a JSON stream
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
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const DeleteFunctionRequest &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H
