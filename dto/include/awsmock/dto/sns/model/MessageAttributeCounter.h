//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_COUNTER_H
#define AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_COUNTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/MessageAttribute.h>
#include <awsmock/dto/sns/model/MessageAttributeDataType.h>

namespace AwsMock::Dto::SNS {

    struct MessageAttributeCounter final : Common::BaseCounter<MessageAttributeCounter> {

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
         * @brief Returns an integer as a byte array and fill it in the given byte array at position offset.
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

      private:

        friend MessageAttributeCounter tag_invoke(boost::json::value_to_tag<MessageAttributeCounter>, boost::json::value const &v) {
            MessageAttributeCounter r;
            r.name = v.at("name").as_string();
            r.stringValue = v.at("stringValue").as_string();
            r.numberValue = v.at("numberValue").as_int64();
            r.type = MessageAttributeDataTypeFromString(v.at("type").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttributeCounter const &obj) {
            jv = {
                    {"name", obj.name},
                    {"stringValue", obj.stringValue},
                    {"numberValue", obj.numberValue},
                    {"type", MessageAttributeDataTypeToString(obj.type)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_COUNTER_H
