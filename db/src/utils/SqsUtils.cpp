//
// Created by vogje01 on 5/1/25.
//

#include <awsmock/utils/SqsUtils.h>

namespace AwsMock::Database {

    std::string SqsUtils::CreateMd5OfMessageBody(const std::string &messageBody) {
        return Core::Crypto::GetMd5FromString(messageBody);
    }

    std::string SqsUtils::CreateMd5OfMessageAttributes(const std::map<std::string, Entity::SQS::MessageAttribute> &attributes) {

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
            UpdateLengthAndBytes(context, MessageAttributeTypeToString(snd.dataType));

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

}// namespace AwsMock::Database