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
         * Attribute binary value
         */
        std::vector<uint8_t> binaryValue;

        /**
         * Attribute binary list values
         */
        std::vector<std::vector<uint8_t> > binaryListValues;

        /**
         * Logical data type
         */
        MessageAttributeDataType dataType{};

    private:
        friend MessageAttribute tag_invoke(boost::json::value_to_tag<MessageAttribute>, boost::json::value const &v) {
            MessageAttribute r;
            r.dataType = MessageAttributeDataTypeFromString(Core::Json::GetStringValue(v, "DataType"));
            r.stringValue = Core::Json::GetStringValue(v, "StringValue");
            if (Core::Json::AttributeExists(v, "StringListValues")) {
                r.stringListValues = boost::json::value_to<std::vector<std::string> >(v.at("StringListValues"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttribute const &obj) {
            boost::json::object o;

            // -------------------------------------------------
            // DataType (always required)
            // -------------------------------------------------
            o["DataType"] = MessageAttributeDataTypeToString(obj.dataType);

            // -------------------------------------------------
            // String type
            // -------------------------------------------------
            if (obj.dataType == STRING) {
                if (!obj.stringValue.empty())
                    o["StringValue"] = obj.stringValue;

                if (!obj.stringListValues.empty())
                    o["StringListValues"] = boost::json::value_from(obj.stringListValues);
            }

            // -------------------------------------------------
            // Binary type
            // -------------------------------------------------
            if (obj.dataType == BINARY) {
                if (!obj.binaryValue.empty())
                    o["BinaryValue"] = boost::json::value_from(obj.binaryValue);

                if (!obj.binaryListValues.empty())
                    o["BinaryListValues"] = boost::json::value_from(obj.binaryListValues);
            }

            jv = std::move(o);
        }
    };

    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

} // namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_H
