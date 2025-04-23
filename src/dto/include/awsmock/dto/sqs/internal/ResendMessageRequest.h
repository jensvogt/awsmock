//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_RESEND_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_RESEND_MESSAGE_REQUEST_H

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
     *   "QueueUrl":"http://localhost:4566/000000000000/artikel-input",
     *   "ReceiptHandle":"string"
     * }
     * @endcode
     */
    struct ResendMessageRequest {

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Message ID
         */
        std::string messageId;

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
        [[nodiscard]] std::string ToJson() const;

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
        friend std::ostream &operator<<(std::ostream &os, ResendMessageRequest &r);
    };
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_RESEND_MESSAGE_REQUEST_H
