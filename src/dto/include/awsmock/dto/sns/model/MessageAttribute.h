//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
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
         * @brief Convert from a BSON string
         *
         * @param jsonObject BSON attribute object
         */
        void FromDocument(const view_or_value<view, value> &jsonObject) {

            try {
                name = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Name");
                stringValue = Core::Bson::BsonUtils::GetStringValue(jsonObject, "StringValue");

                if (!stringValue.empty()) {
                    type = STRING;
                }
                numberValue = Core::Bson::BsonUtils::GetLongValue(jsonObject, "NumberValue");
                if (numberValue > 0) {
                    type = NUMBER;
                }
            } catch (bsoncxx::exception &e) {
                log_error << e.what();
                throw Core::JsonException(e.what());
            }
        }

        /**
         * @brief Convert from a BSON object
         *
         * @return BSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Name", name);
                Core::Bson::BsonUtils::SetStringValue(document, "StringValue", stringValue);
                Core::Bson::BsonUtils::SetLongValue(document, "NumberValue", numberValue);
                Core::Bson::BsonUtils::SetStringValue(document, "DataType", MessageAttributeDataTypeToString(type));
                return document.extract();

            } catch (bsoncxx::exception &e) {
                log_error << e.what();
                throw Core::JsonException(e.what());
            }
        }

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
