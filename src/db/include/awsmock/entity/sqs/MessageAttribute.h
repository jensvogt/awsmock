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
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::SQS {

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
    struct MessageAttribute final : Common::BaseEntity<MessageAttribute> {

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
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        void FromDocument(const view_or_value<view, value> &object);

      private:

        friend MessageAttribute tag_invoke(boost::json::value_to_tag<MessageAttribute>, boost::json::value const &v) {
            MessageAttribute r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.attributeName = v.at("attributeName").as_string();
            r.attributeValue = v.at("attributeValue").as_string();
            r.attributeType = MessageAttributeTypeFromString(v.at("attributeType").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttribute const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"attributeName", obj.attributeName},
                    {"attributeValue", obj.attributeValue},
                    {"attributeType", MessageAttributeTypeToString(obj.attributeType)},
            };
        }
    };

    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_ATTRIBUTE_H
