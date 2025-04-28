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
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttributeDataType.h>

namespace AwsMock::Dto::SQS {

    struct MessageAttribute final : Common::BaseCounter<MessageAttribute> {

        /**
         * Message attribute string value
         */
        std::string stringValue = {};

        /**
         * Message attribute number value
         */
        std::vector<std::string> stringListValues = {};

        /**
         * Message attribute binary value
         */
        unsigned char *binaryValue = nullptr;

        /**
         * Logical data type
         */
        MessageAttributeDataType dataType{};

        /**
         * @brief Returns the MD5 sum of all message attributes (user attributes).
         *
         * @param attributes vector of message attributes
         * @return MD5 sum of message attributes string
         * @see https://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/sqs-message-metadata.html
         */
        static std::string GetMd5Attributes(const std::map<std::string, MessageAttribute> &attributes);

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
         * @brief Convert from JSON document
         *
         * @param jsonObject attribute object
         */
        void FromDocument(const view_or_value<view, value> &jsonObject);

        /**
         * @brief Convert from JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend MessageAttribute tag_invoke(boost::json::value_to_tag<MessageAttribute>, boost::json::value const &v) {
            MessageAttribute r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.stringValue = v.at("stringValue").as_string();
            r.stringListValues = boost::json::value_to<std::vector<std::string>>(v.at("stringListValues"));
            //r.binaryValue = v.at("binaryValue").as_uint64();
            r.dataType = MessageAttributeDataTypeFromString(v.at("dataType").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttribute const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"stringValue", obj.stringValue},
                    {"stringListValues", boost::json::value_from(obj.stringListValues)},
                    {"dataType", MessageAttributeDataTypeToString(obj.dataType)},
            };
        }
    };
    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H
