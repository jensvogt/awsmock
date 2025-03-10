//
// Created by vogje01 on 12/2/23.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_DATA_TYPE_H
#define AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_DATA_TYPE_H

namespace AwsMock::Dto::SQS {

    enum MessageAttributeDataType {
        STRING,
        NUMBER,
        BINARY
    };

    static std::map<MessageAttributeDataType, std::string> MessageAttributeDataTypeNames{
            {STRING, "String"},
            {NUMBER, "Number"},
            {BINARY, "Binary"},
    };

    [[maybe_unused]] static std::string MessageAttributeDataTypeToString(const MessageAttributeDataType &messageAttributeDataType) {
        return MessageAttributeDataTypeNames[messageAttributeDataType];
    }

    [[maybe_unused]] static MessageAttributeDataType MessageAttributeDataTypeFromString(const std::string &messageAttributeDataType) {
        for (auto &[fst, snd]: MessageAttributeDataTypeNames) {
            if (snd == messageAttributeDataType) {
                return fst;
            }
        }
        return STRING;
    }

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ATTRIBUTE_DATA_TYPE_H
