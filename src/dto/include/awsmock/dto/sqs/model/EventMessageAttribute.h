//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_EVENT_MESSAGE_ATTRIBUTE_H
#define AWSMOCK_DTO_SQS_EVENT_MESSAGE_ATTRIBUTE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttributeDataType.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Event message attribute
     *
     * @par
     * The difference between this and a normal MessageAttributes, that here the JSON attributes are in lower case letters
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EventMessageAttribute final : Common::BaseCounter<EventMessageAttribute> {

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

      private:

        friend EventMessageAttribute tag_invoke(boost::json::value_to_tag<EventMessageAttribute>, boost::json::value const &v) {
            EventMessageAttribute r;
            r.dataType = MessageAttributeDataTypeFromString(Core::Json::GetStringValue(v, "dataType"));
            r.stringValue = Core::Json::GetStringValue(v, "stringValue");
            if (Core::Json::AttributeExists(v, "stringListValues")) {
                r.stringListValues = boost::json::value_to<std::vector<std::string>>(v.at("stringListValues"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EventMessageAttribute const &obj) {
            jv = {
                    {"stringValue", obj.stringValue},
                    {"stringListValues", boost::json::value_from(obj.stringListValues)},
                    {"dataType", MessageAttributeDataTypeToString(obj.dataType)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_EVENT_MESSAGE_ATTRIBUTE_H
