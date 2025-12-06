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

    enum MessageAttributeType {
        STRING,
        STRING_LIST,
        BINARY,
        BINARY_LIST,
        UNKNOWN
    };
    static std::map<MessageAttributeType, std::string> MessageAttributeTypeNames{
            {STRING, "String"},
            {STRING_LIST, "StringList"},
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
         * Message attribute value
         */
        MessageAttributeType dataType = STRING;

        /**
         * Attribute binary value
         */
        // TODO: proper implementation of bytes
        //std::vector<std::byte> binaryValue;

        /**
         * Attribute a binary value list
         */
        // TODO: proper implementation of bytes
        //std::vector<std::vector<std::byte>> binaryListValues;

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
        /*
      private:

        friend MessageAttribute tag_invoke(boost::json::value_to_tag<MessageAttribute>, boost::json::value const &v) {
            MessageAttribute r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.stringValue = v.at("stringValue").as_string();
            r.stringListValues = boost::json::value_to<std::vector<std::string>>(v.at("stringValue"));
            // TODO: proper implementation of bytes
            //r.binaryValue = boost::json::value_to<std::vector<std::byte>>(v.at("binaryValue"));
            //r.binaryListValues = boost::json::value_to<std::vector<std::vector<std::byte>>>(v.at("binaryListValues"));
            r.dataType = MessageAttributeTypeFromString(v.at("attributeType").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttribute const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"stringValue", obj.stringValue},
                    {"stringListValue", boost::json::value_from(obj.stringListValues)},
                    // TODO: proper implementation of bytes
                    //{"binaryValue", boost::json::value_from(obj.binaryValue)},
                    //{"binaryListValues", boost::json::value_from(obj.binaryListValues)},
                    {"dataType", MessageAttributeTypeToString(obj.dataType)},
            };
        }*/
    };

    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H
