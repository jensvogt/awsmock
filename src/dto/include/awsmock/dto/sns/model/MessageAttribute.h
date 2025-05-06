//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/MessageAttributeDataType.h>

namespace AwsMock::Dto::SNS {

    struct MessageAttribute final : Common::BaseCounter<MessageAttribute> {

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
        MessageAttributeDataType type = STRING;

        /**
         * @brief Returns the MD5 sum of all attributes (system attributes).
         *
         * @param attributes map of attributes
         * @return MD5 sum of attribute string
         */
        static std::string GetMd5Attributes(const std::map<std::string, std::string> &attributes);

        /**
         * @brief Returns the MD5 sum of all message attributes (user attributes).
         *
         * @param attributes vector of message attributes
         * @return MD5 sum of message attributes string
         */
        static std::string GetMd5MessageAttributes(const std::map<std::string, MessageAttribute> &attributes);

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

      private:

        friend MessageAttribute tag_invoke(boost::json::value_to_tag<MessageAttribute>, boost::json::value const &v) {
            MessageAttribute r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.stringValue = Core::Json::GetStringValue(v, "StringValue");
            r.numberValue = Core::Json::GetLongValue(v, "NumberValue");
            r.type = MessageAttributeDataTypeFromString(Core::Json::GetStringValue(v, "NumberValue"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttribute const &obj) {
            jv = {
                    {"name", obj.name},
                    {"stringValue", obj.stringValue},
                    {"numberValue", obj.numberValue},
                    {"type", MessageAttributeDataTypeToString(obj.type)},
            };
        }
    };

    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_H
