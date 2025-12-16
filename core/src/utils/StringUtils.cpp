
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Core {

    template<typename T = std::mt19937>
    auto RandomGenerator() -> T {
        auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
        auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
        auto seed = std::array<std::seed_seq::result_type, seed_len>();
        auto dev = std::random_device();
        std::generate_n(begin(seed), seed_len, std::ref(dev));
        auto seed_seq = std::seed_seq(begin(seed), end(seed));
        return T{seed_seq};
    }

    auto randomString(const std::size_t len) -> std::string {
        static constexpr auto chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        thread_local auto rng = RandomGenerator<>();
        auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
        auto result = std::string(len, '\0');
        std::generate_n(begin(result), len, [&]() {
            return chars[dist(rng)];
        });
        return result;
    }

    std::string StringUtils::GenerateRandomString(const int length) {
        return randomString(length);
    }

    std::string StringUtils::GenerateRandomAlphanumericString(const int length) {
        return ToLower(randomString(length));
    }

    auto randomHexString(const std::size_t len) -> std::string {
        static constexpr auto chars = "0123456789abcdef";
        thread_local auto rng = RandomGenerator<>();
        auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
        auto result = std::string(len, '\0');
        std::generate_n(begin(result), len, [&]() {
            return chars[dist(rng)];
        });
        return result;
    }

    std::string StringUtils::GenerateRandomHexString(const int length) {
        return randomHexString(length);
    }

    auto randomPasswordString(const std::size_t len) -> std::string {
        static constexpr auto chars = R"(0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~)";
        thread_local auto rng = RandomGenerator<>();
        auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
        auto result = std::string(len, '\0');
        std::generate_n(begin(result), len, [&]() {
            return chars[dist(rng)];
        });
        return result;
    }

    std::string StringUtils::GenerateRandomPassword(const int length) {
        return randomPasswordString(length);
    }

    auto randomVersionString(const std::size_t len) -> std::string {
        static constexpr auto chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        thread_local auto rng = RandomGenerator<>();
        auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
        auto result = std::string(len, '\0');
        std::generate_n(begin(result), len, [&]() {
            return chars[dist(rng)];
        });
        return result;
    }

    std::string StringUtils::GenerateRandomVersion(const int length) {
        return randomVersionString(length);
    }

    bool StringUtils::IsNumeric(const std::string &value) {
        return !value.empty() && std::ranges::find_if(value, [](unsigned char c) {
                                     return !std::isdigit(c);
                                 }) == value.end();
    }

    bool StringUtils::IsUuid(const std::string &value) {
        const std::regex regex("[[:xdigit:]]{8}(-[[:xdigit:]]{4}){3}-[[:xdigit:]]{12}", std::regex_constants::icase);
        return std::regex_match(value, regex);
    }

    bool StringUtils::IsHexString(const std::string &value) {
        return std::ranges::all_of(value, [](const unsigned char c) {
            return !std::isxdigit(c);
        });
    }

    std::string StringUtils::CreateRandomUuid() {
        return to_string(boost::uuids::random_generator()());
    }

    std::vector<std::string> StringUtils::Split(const std::string &s, std::string delim) {
        std::vector<std::string> result;
        boost::split(result, s, boost::is_any_of(delim));
        return result;
    }

    std::vector<std::string> StringUtils::SplitOnNewline(const std::string &s) {
        std::vector<std::string> tokens;
        std::stringstream check1(s);
        std::string intermediate;
        while (getline(check1, intermediate, '\r')) {
            intermediate = StripLineEndings(intermediate);
            tokens.push_back(intermediate);
        }
        return tokens;
    }

    std::string StringUtils::Join(const std::vector<std::string> &vec, const std::string &delimiter, const int startIndex) {
        std::string result;
        for (int i = startIndex; i < vec.size(); i++) {
            result += vec[i];
            if (i != vec.size() - 1) {
                result += delimiter;
            }
        }
        return result;
    }

    std::string StringUtils::StripWhiteSpaces(std::string &s) {
        const std::string &chars = "\t\n\r\v\f ";
        s.erase(std::ranges::remove_if(s, [&chars](const char &c) {
                    return chars.find(c) != std::string::npos;
                }).begin(),
                s.end());
        return s;
    }

    std::string StringUtils::StripLineEndings(std::basic_string<char> s) {
        const std::string &chars = "\n\r";
        s.erase(std::ranges::remove_if(s, [&chars](const char &c) {
                    return chars.find(c) != std::string::npos;
                }).begin(),
                s.end());
        return s;
    }

    std::string StringUtils::StripBeginning(const std::string &s1, const std::string &s2) {
        return s1.substr(s2.length());
    }

    bool StringUtils::Equals(const std::string &s1, const std::string &s2) {
        if (s1.empty() || s2.empty()) {
            return false;
        }
        return s1 == s2;
    }

    bool StringUtils::EqualsIgnoreCase(const std::string &s1, const std::string &s2) {
        return boost::beast::iequals(s1, s2);
    }

    bool StringUtils::IsNullOrEmpty(const std::string *s1) {
        return s1 == nullptr || s1->empty();
    }

    bool StringUtils::Contains(const std::string &s1, const std::string &s2) {
        return s1.find(s2) != std::string::npos;
    }

    bool StringUtils::ContainsIgnoreCase(const std::string &s1, const std::string &s2) {
        return boost::algorithm::to_lower_copy(s1).find(ToLower(s2)) != std::string::npos;
    }

    bool StringUtils::StartsWith(const std::string &s1, const std::string &s2) {
        return s1.starts_with(s2);
    }

    bool StringUtils::StartsWithIgnoringCase(const std::string &s1, const std::string &s2) {
        return boost::algorithm::to_lower_copy(s1).starts_with(boost::algorithm::to_lower_copy(s2));
    }

    bool StringUtils::EndsWith(const std::string &s1, const std::string &s2) {
        return s1.ends_with(s2);
    }

    std::string StringUtils::SubString(const std::string &string, const int beginIndex, const int endIndex) {
        const int size = static_cast<int>(string.size());
        if (beginIndex < 0 || beginIndex > size - 1) return "-1";// Index out of bounds
        if (endIndex < 0 || endIndex > size - 1) return "-1";    // Index out of bounds
        if (beginIndex > endIndex) return "-1";                  // Begin index should not be bigger that end.

        std::string substr;
        for (int i = 0; i < size; i++)
            if (i >= beginIndex && i <= endIndex) substr += (char) string[i];
        return substr;
    }

    std::string StringUtils::SubStringUntil(const std::string &string, const std::string &delimiter) {
        if (Contains(string, delimiter)) {
            return string.substr(0, string.find(delimiter));
        }
        return string;
    }

    std::string StringUtils::SubStringAfter(const std::string &string, const std::string &delimiter) {
        if (Contains(string, delimiter)) {
            return string.substr(string.find(delimiter) + 1);
        }
        return {};
    }

    std::string StringUtils::UrlEncode(const std::string &input) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (const char c: input) {
            if (c == '"' || c == '\'' || c == '!' || c == '*' || c == '(' || c == ')' || c == ';' || c == ':' || c == '@' || c == '&' || c == '=' ||
                c == '+' || c == '$' || c == ',' || c == '?' || c == '#' || c == '[' || c == ']' || c == ' ' || c == '\n' || c == '{' || c == '}' ||
                c == '|' || c == '*') {
                // Any other characters are percent-encoded
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
                escaped << std::nouppercase;
            } else {
                escaped << c;
            }
        }
        return escaped.str();
    }

    void StringUtils::Replace(std::string &target, const std::string &pattern, const std::string &replacement) {
        boost::replace_all(target, pattern, replacement);
    }

    std::string StringUtils::Continuation(const std::string &input, const int length) {
        return input.substr(0, length) + "...";
    }

    std::string StringUtils::UrlDecode(const std::string &input) {
        return boost::to_string(boost::urls::decode_view(input));
    }

    std::string StringUtils::Quoted(const std::string &input) {
        std::stringstream escaped;
        escaped << std::quoted(input);
        return escaped.str();
    }

    std::string StringUtils::ToHexString(const unsigned char *input, const size_t length) {
        std::stringstream ss;
        for (size_t i = 0; i < length; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(input[i]);
        }
        return ss.str();
    }

    std::string StringUtils::ToString(const bool value) {
        return value ? "true" : "false";
    }

    std::string StringUtils::StripChunkSignature(const std::string &input) {
        const std::regex regex("(^|\r\n)[0-9a-fA-F]+;chunk-signature=[0-9a-f]{64}(\r\n)(\r\n$)?", std::regex_constants::icase);
        return std::regex_replace(input, regex, "");
    }

    std::string StringUtils::RemoveColorCoding(const std::string &input) {
        const std::regex regex(R"(\x1b\[[\d;]*?m)", std::regex_constants::icase);
        return std::regex_replace(input, regex, "");
    }

    std::string StringUtils::ToSnakeCase(const std::string &in) {

        // Empty result string
        std::string result;

        // Append first character(in lower case) to result string
        const char c = static_cast<char>(tolower(in[0]));
        result += c;

        // Traverse the string from first index to last index
        for (int i = 1; i < in.length(); i++) {

            // Check if the character is upper case then append '-' and such character (in lower case) to result string
            if (const char ch = in[i]; isupper(ch)) {
                result += '-';
                result += char(tolower(ch));
            } else {
                // If the character is lower case then add such character into result string
                result += ch;
            }
        }

        // Return the result
        return result;
    }

    std::string StringUtils::SanitizeUtf8(const std::string &input) {
        std::string output;
        output.reserve(input.size());

        const auto *s = reinterpret_cast<const unsigned char *>(input.data());
        size_t i = 0;

        while (i < input.size()) {

            if (const unsigned char c = s[i]; c <= 0x7F) {
                // ASCII
                output.push_back(c);
                i++;
            } else if ((c & 0xE0) == 0xC0 && i + 1 < input.size() && (s[i + 1] & 0xC0) == 0x80) {
                output.append(input, i, 2);
                i += 2;
            } else if ((c & 0xF0) == 0xE0 && i + 2 < input.size() && (s[i + 1] & 0xC0) == 0x80 && (s[i + 2] & 0xC0) == 0x80) {
                output.append(input, i, 3);
                i += 3;
            } else if ((c & 0xF8) == 0xF0 && i + 3 < input.size() && (s[i + 1] & 0xC0) == 0x80 && (s[i + 2] & 0xC0) == 0x80 && (s[i + 3] & 0xC0) == 0x80) {
                output.append(input, i, 4);
                i += 4;
            } else {
                // Invalid UTF-8 byte → replace
                output.append("\xEF\xBF\xBD");// Unicode replacement char �
                i++;
            }
        }
        return output;
    }

    std::string StringUtils::RemoveUnprintableAscii(std::string &s) {
        s.erase(std::remove_if(s.begin(), s.end(),
                               [](unsigned char c) {
                                   return !std::isprint(c) && c != '\n' && c != '\t';
                               }),
                s.end());
        return s;
    }

#ifdef _WIN32
    std::wstring StringUtils::ConvertToWideString(const std::string &input) {
        std::wstring wstr(input.length(), 0);
        std::ranges::copy(input, wstr.begin());
        return wstr;
    }

    std::string StringUtils::ConvertToNarrowString(const std::wstring &input) {

        if (input.empty()) return std::string();

        // Calculate the required buffer size
        const int size_needed = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), static_cast<int>(input.length()), nullptr, 0, nullptr, nullptr);

        // Allocate the buffer and perform the conversion
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, input.c_str(), static_cast<int>(input.length()), &str[0], size_needed, nullptr, nullptr);
        return str;
    }
#endif

}// namespace AwsMock::Core