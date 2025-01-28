//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_UPDATE_DQL_REQUEST_H
#define AWSMOCK_DTO_SQS_UPDATE_DQL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Update message request
     *
     * Example (from Java v2 client):
     * @code{.json}
     * {
     *   "QueueArn":"string",
     *   "TargetArn":"string",
     *   "Retries": number
     * }
     * @endcode
     */
    struct UpdateDqlRequest {

        /**
         * Source queue ARN
         */
        std::string queueArn;

        /**
         * Target ARN
         */
        std::string targetArn;

        /**
         * Max number of receives
         */
        int retries;

        /**
         * @brief Converts the JSON string to a DTO
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson();

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString();

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, UpdateDqlRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_UPDATE_DQL_REQUEST_H
