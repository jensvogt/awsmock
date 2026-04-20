//
// Created by vogje01 on 5/1/25.
//

#include <awsmock/utils/SqsUtils.h>

#include "awsmock/entity/sns/MessageAttribute.h"

namespace AwsMock::Database {

    std::string SqsUtils::CreateMd5OfMessageBody(const std::string &messageBody) {
        return Core::Crypto::GetMd5FromString(messageBody);
    }

    std::string SqsUtils::CreateMd5OfMessageSystemAttributes(const std::map<std::string, std::string> &attributes) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        const EVP_MD *md = EVP_md5();
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        auto *bytes = new unsigned char[1];

        EVP_DigestInit(context, md);
        for (const auto &[fst, snd]: attributes) {

            log_debug << "MD5sum, attribute: " << fst;

            // Encoded name
            UpdateLengthAndBytes(context, fst);

            // Encoded data type
            UpdateLengthAndBytes(context, "String");

            // Encoded value
            if (!snd.empty()) {
                bytes[0] = 1;
                EVP_DigestUpdate(context, bytes, 1);

                // Url decode the attribute
                UpdateLengthAndBytes(context, Core::StringUtils::UrlDecode(snd));
            }
        }
        EVP_DigestFinal(context, md_value, &md_len);
        EVP_MD_CTX_free(context);
        delete[] bytes;

        return Core::Crypto::HexEncode(md_value, static_cast<int>(md_len));
    }

    std::string SqsUtils::CreateMd5OfMessageAttributes(
        const std::map<std::string, Entity::SQS::MessageAttribute> &messageAttributes) {

        EVP_MD_CTX *context = EVP_MD_CTX_new();
        EVP_DigestInit(context, EVP_md5());

        for (const auto &[name, attr]: messageAttributes) {

            log_debug << "MD5sum, attribute: " << name;

            // 1. Encode name
            UpdateLengthAndBytes(context, name);

            // 2. Encode data type (e.g. "String", "Number", "Binary")
            const std::string dataType = MessageAttributeTypeToString(attr.dataType);
            UpdateLengthAndBytes(context, dataType);

            // 3. Transport type byte + value
            if (attr.dataType == Entity::SQS::STRING || attr.dataType == Entity::SQS::NUMBER) {
                uint8_t transportType = 0x01;
                EVP_DigestUpdate(context, &transportType, 1);
                UpdateLengthAndBytes(context, attr.stringValue);

            } else if (attr.dataType == Entity::SQS::BINARY) {
                // Binary: transport type = 0x02
                uint8_t transportType = 0x02;
                EVP_DigestUpdate(context, &transportType, 1);
                // binary value: length + raw bytes
                uint32_t len = htonl(static_cast<uint32_t>(attr.binaryValue.size()));
                EVP_DigestUpdate(context, &len, sizeof(len));
                EVP_DigestUpdate(context, attr.binaryValue.data(), attr.binaryValue.size());
            }
        }

        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        EVP_DigestFinal(context, md_value, &md_len);
        EVP_MD_CTX_free(context);

        return Core::Crypto::HexEncode(md_value, static_cast<int>(md_len));
    }

    void SqsUtils::GetIntAsByteArray(const size_t n, unsigned char *bytes) {
        if (bytes) {
            bytes[3] = n & 0x000000ff;
            bytes[2] = (n & 0x0000ff00) >> 8;
            bytes[1] = (n & 0x00ff0000) >> 16;
            bytes[0] = (n & 0xff000000) >> 24;
        }
    }

    void SqsUtils::UpdateLengthAndBytes(EVP_MD_CTX *context, const std::string &str) {
        auto *bytes = static_cast<unsigned char *>(malloc(4));
        GetIntAsByteArray(str.length(), bytes);
        EVP_DigestUpdate(context, bytes, 4);
        EVP_DigestUpdate(context, str.c_str(), str.length());
        free(bytes);
    }

} // namespace AwsMock::Database
