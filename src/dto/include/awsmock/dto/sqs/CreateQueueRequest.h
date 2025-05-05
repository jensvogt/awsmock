//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_CREATE_QUEUE_REQUEST_H
#define AWSMOCK_DTO_SQS_CREATE_QUEUE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/sqs/model/QueueAttribute.h>

namespace AwsMock::Dto::SQS {

    struct CreateQueueRequest {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Queue name
         */
        std::string queueName;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Attributes
         */
        QueueAttributeList attributes;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * AWS request ID
         */
        std::string requestId;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the JSON string to DTO.
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
        friend std::ostream &operator<<(std::ostream &os, const CreateQueueRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_CREATE_QUEUE_REQUEST_H
