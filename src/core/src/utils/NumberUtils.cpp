//
//
//

#include <awsmock/core/NumberUtils.h>

namespace AwsMock::Core {

    std::string NumberUtils::ToKilobytesStr(long value) {
        return std::to_string(ToKilobytes(value)) + "k";
    }

    long NumberUtils::ToKilobytes(long value) {
        return value / 1024;
    }

    std::string NumberUtils::ToMegabytesStr(long value) {
        return std::to_string(ToMegabytes(value)) + "m";
    }

    long NumberUtils::ToMegabytes(long value) {
        return value / 1024 / 1024;
    }

    void NumberUtils::GetIntAsByteArray(int n, unsigned char *bytes, int offset) {
        bytes[offset + 3] = n & 0x000000ff;
        bytes[offset + 2] = (n & 0x0000ff00) >> 8;
        bytes[offset + 1] = (n & 0x00ff0000) >> 16;
        bytes[offset] = (n & 0xff000000) >> 24;
    }

    int NumberUtils::HexToInt(const std::string &hex) {
        return static_cast<int>(std::stoul(hex, nullptr, 16));
    }

    int NumberUtils::ToInt(const std::string &str) {
        if (!str.empty()) {
            return std::stoi(str);
        }
        return -1;
    }

    double NumberUtils::ToDouble(const std::string &str) {
        if (!str.empty()) {
            return std::stod(str);
        }
        return -1.0;
    }

}// namespace AwsMock::Core