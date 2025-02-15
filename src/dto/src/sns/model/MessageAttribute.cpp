//
// Created by vogje01 on 23/09/2023.
//

#include "awsmock/dto/sns/model/MessageAttribute.h"

namespace AwsMock::Dto::SNS {

    bool MessageAttribute::operator<(const MessageAttribute &other) const {
        return name < other.name;
    }

    std::string MessageAttribute::GetMd5Attributes(const std::map<std::string, std::string> &attributes) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        auto *bytes = new unsigned char[1];

        EVP_DigestInit(context, md);
        for (const auto &[fst, snd]: attributes) {

            // Encoded name
            UpdateLengthAndBytes(context, fst);

            // Encoded value
            if (!snd.empty()) {
                bytes[0] = 1;
                EVP_DigestUpdate(context, bytes, 1);
                UpdateLengthAndBytes(context, Core::StringUtils::UrlDecode(snd));
            }
        }
        EVP_DigestFinal(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] bytes;

        std::string output;
        output.resize(md_len * 2);
        for (unsigned int i = 0; i < md_len; ++i) {
            std::snprintf(&output[i * 2], md_len * 2, "%02x", md_value[i]);
        }

        return output;
    }

    std::string MessageAttribute::GetMd5MessageAttributes(const std::map<std::string, MessageAttribute> &attributes, const bool includeContentType) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        auto *bytes = new unsigned char[1];

        EVP_DigestInit(context, md);
        for (const auto &[fst, snd]: attributes) {

            if (fst == "contentType" && !includeContentType) {
                continue;
            }

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

    std::string MessageAttribute::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MessageAttribute &r) {
        os << "MessageAttribute=" << to_json(r.ToDocument());
        return os;
    }
}// namespace AwsMock::Dto::SNS
