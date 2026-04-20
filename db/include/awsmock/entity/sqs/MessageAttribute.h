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

// AwsMock includes
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Database::Entity::SQS {

    enum MessageAttributeType {
        STRING,
        STRING_LIST,
        NUMBER,
        NUMBER_LIST,
        BINARY,
        BINARY_LIST,
        UNKNOWN
    };

    static std::map<MessageAttributeType, std::string> MessageAttributeTypeNames{
        {STRING, "String"},
        {STRING_LIST, "StringList"},
        {NUMBER, "Number"},
        {NUMBER_LIST, "NumberList"},
        {BINARY, "Binary"},
        {BINARY_LIST, "BinaryList"},
        {UNKNOWN, "Unknown"},
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
        return UNKNOWN;
    }

    /**
     * SQS message attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MessageAttribute /*final : Common::BaseEntity<MessageAttribute>*/ {

        /**
         * Attribute string value
         */
        std::string stringValue;

        /**
         * Attribute string list values
         */
        std::vector<std::string> stringListValues;

        /**
         * Attribute binary value
         */
        std::vector<uint8_t> binaryValue;

        /**
         * Attribute binary list values
         */
        std::vector<std::vector<uint8_t> > binaryListValues;

        /**
         * Message attribute value
         */
        MessageAttributeType dataType = STRING;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        void FromDocument(const view_or_value<view, value> &object);
    };

    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

} // namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H
