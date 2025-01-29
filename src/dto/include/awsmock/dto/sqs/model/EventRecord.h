//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_EVENT_RECORD_H
#define AWSMOCK_DTO_SQS_EVENT_RECORD_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    struct Record {

        /**
         * Region
         */
        std::string region;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Receipt handle
         */
        std::string receiptHandle;

        /**
         * Body
         */
        std::string body;

        /**
         * System attributes
         */
        std::map<std::string, std::string> attributes;

        /**
         * Message attributes
         */
        MessageAttributeList messagesAttributes;

        /**
         * MD5 sum
         */
        std::string md5Sum;

        /**
         * Event source
         */
        std::string eventSource;

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @param document DTO as BSON document
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * @brief Converts the DTO to a JSON string.
         *
         * @return DTO as JSON string.
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
        friend std::ostream &operator<<(std::ostream &os, const Record &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_EVENT_RECORD_H
