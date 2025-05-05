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
#include <awsmock/core/JsonUtils.h>
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
            r.dataType = MessageAttributeDataTypeFromString(Core::Json::GetStringValue(v, "DataType"));
            r.stringValue = Core::Json::GetStringValue(v, "StringValue");
            if (Core::Json::AttributeExists(v, "StringListValues")) {
                r.stringListValues = boost::json::value_to<std::vector<std::string>>(v.at("StringListValues"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttribute const &obj) {
            jv = {
                    {"StringValue", obj.stringValue},
                    {"StringListValues", boost::json::value_from(obj.stringListValues)},
                    {"DataType", MessageAttributeDataTypeToString(obj.dataType)},
            };
        }
    };
    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H
