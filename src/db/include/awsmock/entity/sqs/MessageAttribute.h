//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H

// C++ includes
#include <map>
#include <string>
#include <utility>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Database::Entity::SQS {

    using bsoncxx::view_or_value;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::document::value;
    using bsoncxx::document::view;

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
     * SQS message attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MessageAttribute {

        /**
         * Message attribute name
         */
        std::string attributeName;

        /**
         * Message attribute value
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
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
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
         * @param os output stream
         * @param m message attribute
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const MessageAttribute &m);
    };

    typedef std::vector<MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H
