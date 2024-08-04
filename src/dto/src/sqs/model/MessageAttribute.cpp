//
// Created by vogje01 on 23/09/2023.
//

#include "awsmock/dto/sqs/model/MessageAttribute.h"

namespace AwsMock::Dto::SQS {

    bool MessageAttribute::operator<(const AwsMock::Dto::SQS::MessageAttribute &other) const {
        return name < other.name;
    }

    std::string MessageAttribute::GetMd5Attributes(const std::map<std::string, std::string> &attributes) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        auto *bytes = new unsigned char[1];

        EVP_DigestInit(context, md);
        for (const auto &a: attributes) {

            // Encoded name
            UpdateLengthAndBytes(context, a.first);

            // Encoded data type
            //UpdateLengthAndBytes(context, Dto::SQS::MessageAttributeDataTypeToString(a.second));

            // Encoded value
            if (!a.second.empty()) {
                bytes[0] = 1;
                EVP_DigestUpdate(context, bytes, 1);
                UpdateLengthAndBytes(context, a.second);
            }
        }
        EVP_DigestFinal(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] bytes;

        std::string output;
        output.resize(md_len * 2);
        for (unsigned int i = 0; i < md_len; ++i) {
            std::sprintf(&output[i * 2], "%02x", md_value[i]);
        }

        return output;
    }

    std::string MessageAttribute::GetMd5MessageAttributes(const std::map<std::string, MessageAttribute> &attributes) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        auto *bytes = new unsigned char[1];

        EVP_DigestInit(context, md);
        for (const auto &a: attributes) {
            //if (!a.second.systemAttribute) {
            // Encoded name
            UpdateLengthAndBytes(context, a.first);

            // Encoded data type
            UpdateLengthAndBytes(context, Dto::SQS::MessageAttributeDataTypeToString(a.second.type));

            // Encoded value
            if (!a.second.stringValue.empty()) {
                bytes[0] = 1;
                EVP_DigestUpdate(context, bytes, 1);
                UpdateLengthAndBytes(context, a.second.stringValue);
            }
            //}
        }
        EVP_DigestFinal(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] bytes;

        return Core::Crypto::HexEncode(md_value, md_len);
    }

    void MessageAttribute::GetIntAsByteArray(size_t n, unsigned char *bytes) {
        bytes[3] = n & 0x000000ff;
        bytes[2] = (n & 0x0000ff00) >> 8;
        bytes[1] = (n & 0x00ff0000) >> 16;
        bytes[0] = (n & 0xff000000) >> 24;
    }

    void MessageAttribute::UpdateLengthAndBytes(EVP_MD_CTX *context, const std::string &str) {
        auto *bytes = static_cast<unsigned char *>(malloc(4));
        GetIntAsByteArray(str.length(), bytes);
        EVP_DigestUpdate(context, bytes, 4);
        EVP_DigestUpdate(context, str.c_str(), str.length());
        free(bytes);
    }

    void MessageAttribute::FromJsonObject(const Poco::JSON::Object::Ptr &jsonObject) {

        Core::JsonUtils::GetJsonValueString("Name", jsonObject, name);
        Core::JsonUtils::GetJsonValueString("StringValue", jsonObject, stringValue);
        if (!stringValue.empty()) {
            type = MessageAttributeDataType::STRING;
        }
        Core::JsonUtils::GetJsonValueLong("NumberValue", jsonObject, numberValue);
        if (numberValue > 0) {
            type = MessageAttributeDataType::NUMBER;
        }
    }

    Poco::JSON::Object MessageAttribute::ToJsonObject() const {

        Poco::JSON::Object jsonObject;
        jsonObject.set("Name", name);
        jsonObject.set("StringValue", stringValue);
        jsonObject.set("NumberValue", numberValue);
        jsonObject.set("DataType", MessageAttributeDataTypeToString(type));
        return jsonObject;
    }

    std::string MessageAttribute::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MessageAttribute &r) {
        os << "MessageAttribute={name='" << r.name << "', type='" << MessageAttributeDataTypeToString(r.type) << "', stringValue='" << r.stringValue << "', numberValue="
           << r.numberValue;
        if (r.binaryValue != nullptr) {
            os << ", binaryValue='" << r.binaryValue << "'";
        }
        os << "}";
        return os;
    }
}// namespace AwsMock::Dto::SQS
