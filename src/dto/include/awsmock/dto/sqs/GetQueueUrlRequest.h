//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_URL_REQUEST_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_URL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/ServiceException.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Get SQS queue URL request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetQueueUrlRequest {

        /**
         * AWS region name
         */
        std::string region;

        /**
         * Name of the queue
         */
        std::string queueName;

        /**
         * Converts the JSON string to a DTO
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
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const GetQueueUrlRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_URL_REQUEST_H
