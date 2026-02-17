//
// Created by vogje01 on 2/17/26.
//

#ifndef AWSMOCK_CRC32PROCESSOR_H
#define AWSMOCK_CRC32PROCESSOR_H

#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <vector>

namespace AwsMock::Core::Crypto {

    class CRC32Processor {

        uint32_t table[256]{};

        // A simple Base64 encoder for the 4-byte CRC
        static std::string base64_encode_bytes(const uint8_t *data, size_t len) {
            static constexpr char lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string out;
            int val = 0, valb = -6;
            for (size_t i = 0; i < len; ++i) {
                val = (val << 8) + data[i];
                valb += 8;
                while (valb >= 0) {
                    out.push_back(lookup[(val >> valb) & 0x3F]);
                    valb -= 6;
                }
            }
            if (valb > -6) out.push_back(lookup[((val << 8) >> (valb + 8)) & 0x3F]);
            while (out.size() % 4) out.push_back('=');
            return out;
        }

        static std::string get_s3_crc32_header(uint32_t crc32_val) {
            // 1. S3 expects Big-Endian (Network Byte Order)
            const uint32_t network_order = htonl(crc32_val);

            // 2. Treat the integer as a raw 4-byte array
            const auto *bytes = reinterpret_cast<const uint8_t *>(&network_order);

            // 3. Base64 encode those 4 bytes
            return base64_encode_bytes(bytes, 4);
        }

      public:

        CRC32Processor() {
            for (uint32_t i = 0; i < 256; i++) {
                uint32_t remainder = i;
                for (uint32_t j = 0; j < 8; j++) {
                    if (remainder & 1) {
                        constexpr uint32_t polynomial = 0xEDB88320;
                        remainder = (remainder >> 1) ^ polynomial;
                    } else {
                        remainder >>= 1;
                    }
                }
                table[i] = remainder;
            }
        }

        std::string update(const uint8_t *data, size_t length) const {
            uint32_t crc = 0xFFFFFFFF;
            for (size_t i = 0; i < length; i++) {
                const uint8_t index = (crc ^ data[i]) & 0xFF;
                crc = (crc >> 8) ^ table[index];
            }
            return get_s3_crc32_header(crc ^ 0xFFFFFFFF);
        }
    };
    /*
    int main() {
        CRC32 crcProcessor;
        std::string msg = "Hello S3!";
        uint32_t result = crcProcessor.update((const uint8_t *) msg.c_str(), msg.length());

        std::cout << "Manual CRC32: " << std::hex << result << std::endl;
    }*/
}// namespace AwsMock::Core::Crypto

#endif//AWSMOCK_CRC32PROCESSOR_H
