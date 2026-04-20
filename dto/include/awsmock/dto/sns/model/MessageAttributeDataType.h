//
// Created by vogje01 on 12/2/23.
//

#ifndef AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_DATA_TYPE_H
#define AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_DATA_TYPE_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::SNS {

    enum MessageAttributeDataType {
        STRING,
        STRING_LIST,
        NUMBER,
        NUMBER_LIST,
        BINARY,
        BINARY_LIST
    };

    static std::map<MessageAttributeDataType, std::string> MessageAttributeDataTypeNames{
        {STRING, "String"},
        {STRING_LIST, "StringList"},
        {NUMBER, "Number"},
        {NUMBER_LIST, "NumberList"},
        {BINARY, "Binary"},
        {BINARY_LIST, "BinaryList"},
    };

    [[maybe_unused]] static std::string MessageAttributeDataTypeToString(const MessageAttributeDataType messageAttributeDataType) {
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

} // namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MESSAGE_ATTRIBUTE_DATA_TYPE_H
