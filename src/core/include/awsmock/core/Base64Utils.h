#ifndef AWS_MOCK_CORE_BASE64_UTILS_H
#define AWS_MOCK_CORE_BASE64_UTILS_H

// C++ standard includes
#include <cstdint>
#include <fstream>
#include <string>

// Awsmock includes
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    class Base64Utils {
      public:

        static std::string Encode(const std::string &data) {
            static constexpr char sEncodingTable[] = {
                    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

            const size_t in_len = data.size();
            const size_t out_len = 4 * ((in_len + 2) / 3);
            std::string ret(out_len, '\0');
            size_t i;
            auto p = const_cast<char *>(ret.c_str());

            for (i = 0; in_len > 2 && i < in_len - 2; i += 3) {
                *p++ = sEncodingTable[data[i] >> 2 & 0x3F];
                *p++ = sEncodingTable[(data[i] & 0x3) << 4 | (data[i + 1] & 0xF0) >> 4];
                *p++ = sEncodingTable[(data[i + 1] & 0xF) << 2 | (data[i + 2] & 0xC0) >> 6];
                *p++ = sEncodingTable[data[i + 2] & 0x3F];
            }
            if (i < in_len) {
                *p++ = sEncodingTable[data[i] >> 2 & 0x3F];
                if (i == in_len - 1) {
                    *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
                    *p++ = '=';
                } else {
                    *p++ = sEncodingTable[(data[i] & 0x3) << 4 | (data[i + 1] & 0xF0) >> 4];
                    *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
                }
                *p++ = '=';
            }

            return ret;
        }

        static std::string Decode(const std::string &input, std::string &out) {
            static constexpr unsigned char kDecodingTable[] = {
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57,
                    58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6,
                    7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
                    25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
                    37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64};

            size_t in_len = input.size();
            if (in_len % 4 != 0)
                return "Input data size is not a multiple of 4";

            size_t out_len = in_len / 4 * 3;
            if (in_len >= 1 && input[in_len - 1] == '=')
                out_len--;
            if (in_len >= 2 && input[in_len - 2] == '=')
                out_len--;

            out.resize(out_len);

            for (size_t i = 0, j = 0; i < in_len;) {
                const uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                const uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                const uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                const uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

                const uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

                if (j < out_len)
                    out[j++] = triple >> 2 * 8 & 0xFF;
                if (j < out_len)
                    out[j++] = triple >> 1 * 8 & 0xFF;
                if (j < out_len)
                    out[j++] = triple >> 0 * 8 & 0xFF;
            }

            return "";
        }

        static void DecodeToFile(const std::string &input, const std::string &filename) {
            static constexpr unsigned char kDecodingTable[] = {
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57,
                    58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6,
                    7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
                    25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
                    37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                    64, 64, 64, 64};

            std::ofstream ofs(filename, std::ios::out | std::ios::binary);

            const size_t in_len = input.size();
            if (in_len % 4 != 0) {
                //log_error << "Input data size is not a multiple of 4";
                return;
            }

            size_t out_len = in_len / 4 * 3;
            if (in_len >= 1 && input[in_len - 1] == '=')
                out_len--;
            if (in_len >= 2 && input[in_len - 2] == '=')
                out_len--;

            for (size_t i = 0, j = 0; i < in_len;) {
                const uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                const uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                const uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                const uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

                const uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

                if (j < out_len)
                    ofs << (triple >> 2 * 8 & 0xFF);
                if (j < out_len)
                    ofs << (triple >> 1 * 8 & 0xFF);
                if (j < out_len)
                    ofs << (triple >> 0 * 8 & 0xFF);
            }
            ofs.close();
        }
    };

}// namespace AwsMock::Core

#endif /* AWS_MOCK_CORE_BASE64_UTILS_H */