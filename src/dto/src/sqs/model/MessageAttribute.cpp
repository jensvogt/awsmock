//
// Created by vogje01 on 23/09/2023.
//

#include "awsmock/dto/sqs/model/MessageAttribute.h"

namespace AwsMock::Dto::SQS {

    bool MessageAttribute::operator<(const MessageAttribute &other) const {
        return name < other.name;
    }

    std::string MessageAttribute::GetMd5Attributes(const std::map<std::string, MessageAttribute> &attributes) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        auto *bytes = new unsigned char[1];

        EVP_DigestInit(context, md);
        for (const auto &[fst, snd]: attributes) {

            // Encoded name
            UpdateLengthAndBytes(context, fst);

            // Encoded data type
            UpdateLengthAndBytes(context, MessageAttributeDataTypeToString(snd.type));

            // Encoded value
            if (!snd.stringValue.empty()) {
                bytes[0] = 1;
                EVP_DigestUpdate(context, bytes, 1);

                // Url decode the attribute
                UpdateLengthAndBytes(context, Core::StringUtils::UrlDecode(snd.stringValue));
            }
        }
        EVP_DigestFinal(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] bytes;

        return Core::Crypto::HexEncode(md_value, static_cast<int>(md_len));
    }

    void MessageAttribute::GetIntAsByteArray(const size_t n, unsigned char *bytes) {
        if (bytes) {
            bytes[3] = n & 0x000000ff;
            bytes[2] = (n & 0x0000ff00) >> 8;
            bytes[1] = (n & 0x00ff0000) >> 16;
            bytes[0] = (n & 0xff000000) >> 24;
        }
    }

    void MessageAttribute::UpdateLengthAndBytes(EVP_MD_CTX *context, const std::string &str) {
        auto *bytes = static_cast<unsigned char *>(malloc(4));
        GetIntAsByteArray(str.length(), bytes);
        EVP_DigestUpdate(context, bytes, 4);
        EVP_DigestUpdate(context, str.c_str(), str.length());
        free(bytes);
    }

    void MessageAttribute::FromDocument(const view_or_value<view, value> &jsonObject) {

        try {
            name = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Name");
            type = MessageAttributeDataTypeFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "DataType"));
            if (type == STRING || type == NUMBER) {
                stringValue = Core::Bson::BsonUtils::GetStringValue(jsonObject, "StringValue");
            }

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    void MessageAttribute::FromJson(const view_or_value<view, value> &jsonObject) {

        try {
            type = MessageAttributeDataTypeFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "DataType"));
            if (type == STRING || type == NUMBER) {
                stringValue = Core::Bson::BsonUtils::GetStringValue(jsonObject, "StringValue");
            }

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    view_or_value<view, value> MessageAttribute::ToDocument() const {

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

    view_or_value<view, value> MessageAttribute::ToJson() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "StringValue", stringValue);
            Core::Bson::BsonUtils::SetLongValue(document, "NumberValue", numberValue);
            Core::Bson::BsonUtils::SetStringValue(document, "DataType", MessageAttributeDataTypeToString(type));
            return document.extract();

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    std::string MessageAttribute::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MessageAttribute &r) {
        os << "MessageAttribute=" << to_json(r.ToDocument());
        return os;
    }
}// namespace AwsMock::Dto::SQS
