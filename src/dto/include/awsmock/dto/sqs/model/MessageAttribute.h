//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/dto/sqs/model/MessageAttributeDataType.h>

namespace AwsMock::Dto::SQS {

    struct MessageAttribute {

        /**
         * Message attribute name
         */
        std::string name;

        /**
         * Message attribute string value
         */
        std::string stringValue = {};

        /**
         * Message attribute number value
         */
        long numberValue = -1;

        /**
         * Message attribute binary value
         */
        unsigned char *binaryValue = nullptr;

        /**
         * Logical data type
         */
        MessageAttributeDataType type;

        /**
         * System attribute flag
         */
        bool systemAttribute = true;

        /**
         * @brief Returns the MD5 sum of all attributes (system attributes).
         *
         * @param attributes map of attributes
         * @return MD5 sum of attributes string
         */
        static std::string GetMd5Attributes(const std::map<std::string, std::string> &attributes);

        /**
         * @brief Returns the MD5 sum of all message attributes (user attributes).
         *
         * @param attributes vector of message attributes
         * @param includeContentType if true contentType is included
         * @return MD5 sum of message attributes string
         */
        static std::string GetMd5MessageAttributes(const std::map<std::string, MessageAttribute> &attributes, bool includeContentType);

        /**
         * @brief Update the MD5 hash with a given value
         *
         * @param context MD5 hash model
         * @param str string to append
         */
        static void UpdateLengthAndBytes(EVP_MD_CTX *context, const std::string &str);

        /**
         * @brief Returns an integer as byte array and fill it in the given byte array at position offset.
         *
         * @param n integer value
         * @param bytes output byte array
         */
        static void GetIntAsByteArray(size_t n, unsigned char *bytes);

        /**
         * @brief Convert from JSON string
         *
         * @param attributeObject attribute object
         */
        void FromDocument(const view_or_value<view, value> &attributeObject);


        /**
         * @brief Convert from JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Name comparator
         *
         * @param other
         * @return
         */
        bool operator<(const MessageAttribute &other) const;

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
        friend std::ostream &operator<<(std::ostream &os, const MessageAttribute &r);
    };

    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H
