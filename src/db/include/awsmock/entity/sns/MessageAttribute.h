//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SNS_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DB_ENTITY_SNS_MESSAGE_ATTRIBUTE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/entity/sqs/MessageAttribute.h>

namespace AwsMock::Database::Entity::SNS {

    enum MessageAttributeType {
        STRING,
        NUMBER,
        BINARY
    };
    static std::map<MessageAttributeType, std::string> MessageAttributeTypeNames{
            {STRING, "String"},
            {NUMBER, "Number"},
            {BINARY, "Binary"},
    };

    [[maybe_unused]] static std::string MessageAttributeTypeToString(MessageAttributeType messageAttributeType) {
        return MessageAttributeTypeNames[messageAttributeType];
    }

    [[maybe_unused]] static MessageAttributeType MessageAttributeTypeFromString(const std::string &messageAttributeType) {
        for (auto &[fst, snd]: MessageAttributeTypeNames) {
            if (snd == messageAttributeType) {
                return fst;
            }
        }
        return STRING;
    }

    /**
     * SNS message attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MessageAttribute {

        /**
         * MessageAttribute name
         */
        std::string attributeName;

        /**
         * MessageAttribute value
         */
        std::string attributeValue;

        /**
         * Message attribute value
         */
        MessageAttributeType attributeType;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the entity to a JSON object
         *
         * @return DTO as JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument();

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param m message attribute entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const MessageAttribute &m);
    };

    typedef std::vector<MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Database::Entity::SNS

#endif// AWSMOCK_DB_ENTITY_SNS_MESSAGE_ATTRIBUTE_H
