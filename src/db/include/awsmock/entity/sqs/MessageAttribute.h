//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H

// C++ includes
#include <map>
#include <sstream>
#include <string>
#include <vector>

// Poco includes
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/stdx.hpp>

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
            {MessageAttributeType::STRING, "String"},
            {MessageAttributeType::NUMBER, "Number"},
            {MessageAttributeType::BINARY, "Binary"},
    };

    [[maybe_unused]] static std::string MessageAttributeTypeToString(MessageAttributeType messageAttributeType) {
        return MessageAttributeTypeNames[messageAttributeType];
    }

    [[maybe_unused]] static MessageAttributeType MessageAttributeTypeFromString(const std::string &messageAttributeType) {
        for (auto &it: MessageAttributeTypeNames) {
            if (it.second == messageAttributeType) {
                return it.first;
            }
        }
        return MessageAttributeType::STRING;
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
         * System attribute flag
         */
        bool systemAttribute = true;

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @param os output stream
         * @param m message attribute
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const MessageAttribute &m);
    };

    typedef struct MessageAttribute MessageAttribute;
    typedef std::vector<MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H
