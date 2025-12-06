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
         * Message attribute string value
         */
        std::string stringValue = {};

        /**
         * Message attribute binary value
         */
        unsigned char *binaryValue = nullptr;

        /**
         * Logical data type
         */
        MessageAttributeDataType dataType = STRING;

        /**
         * @brief Convert from a BSON string
         *
         * @param jsonObject BSON attribute object
         */
        void FromDocument(const view_or_value<view, value> &jsonObject) {

            try {
                stringValue = Core::Bson::BsonUtils::GetStringValue(jsonObject, "stringValue");
                if (!stringValue.empty()) {
                    dataType = STRING;
                }
                dataType = MessageAttributeDataTypeFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "dataType"));
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
                document rootDocument;
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "StringValue", stringValue);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "DataType", MessageAttributeDataTypeToString(dataType));
                return rootDocument.extract();

            } catch (bsoncxx::exception &e) {
                log_error << e.what();
                throw Core::JsonException(e.what());
            }
        }

      private:

        friend MessageAttribute tag_invoke(boost::json::value_to_tag<MessageAttribute>, boost::json::value const &v) {
            MessageAttribute r;
            r.stringValue = Core::Json::GetStringValue(v, "StringValue");
            r.dataType = MessageAttributeDataTypeFromString(Core::Json::GetStringValue(v, "DataType"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageAttribute const &obj) {
            jv = {
                    {"StringValue", obj.stringValue},
                    {"DataType", MessageAttributeDataTypeToString(obj.dataType)},
            };
        }
    };

    typedef std::map<std::string, MessageAttribute> MessageAttributeList;

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_H
