//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWSMOCK_CORE_STRING_UTILS_H
#define AWSMOCK_CORE_STRING_UTILS_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>
#endif

// C++ standard includes
#include <iomanip>
#include <random>
#include <regex>
#include <string>
#include <vector>

// Boost includes
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/exception/info.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/encode.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/token_functions.hpp>
#include <boost/tokenizer.hpp>

// JWT includes
#include <jwt-cpp/base.h>

namespace AwsMock::Core {

    /**
     * @brief String utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class StringUtils {

    public:

        /**
         * @brief Generates a random string with the given length.
         *
         * @param length length of the string
         * @return generated random string with the given length.
         */
        static std::string GenerateRandomString(int length);

        /**
         * @brief Generates a random string with the given length.
         *
         * @param length length of the string
         * @return generated random string with the given length.
         */
        static std::string GenerateRandomAlphanumericString(int length);

        /**
         * @brief Generates a random hexadecimal string with the given length.
         *
         * @param length length of the string
         * @return generated random hexadecimal string with the given length.
         */
        static std::string GenerateRandomHexString(int length);

        /**
         * @brief Generates a random password string with the given length.
         *
         * @param length length of the string
         * @return generated random password string with the given length.
         */
        static std::string GenerateRandomPassword(int length);

        /**
         * @brief Generates a random version string with the given length.
         *
         * @param length length of the string
         * @return generated a random password string with the given length.
         */
        static std::string GenerateRandomVersion(int length);

        /**
         * @brief Check whether the provided string is a number.
         *
         * @param value string value
         * @return true in case the provided string is numeric, otherwise false.
         */
        static bool IsNumeric(const std::string &value);

        /**
         * @brief Check whether the provided string is a UUID.
         *
         * @param value string value
         * @return true in case the provided string is a UUID, otherwise false.
         */
        static bool IsUuid(const std::string &value);

        /**
         * @brief Check whether the provided string is a hexadecimal digit.
         *
         * @param value string value
         * @return true in case the provided string is a hexadecimal digit, otherwise false.
         */
        static bool IsHexString(const std::string &value);

        /**
         * @brief Creates a random UUID and return is as a string
         *
         * @return random UUID as a string
         */
        static std::string CreateRandomUuid();

        /**
         * @brief Splits a string into pieces using the provided delimiter char.
         *
         * @param s string to split
         * @param delim delimiters
         * @return vector os strings.
         */
        static std::vector<std::string> Split(const std::string &s, std::string delim);

        /**
         * @brief Splits a string into pieces using the provided delimiter char.
         *
         * @param input string to split
         * @return vector os strings.
         */
        static std::vector<std::string> SplitOnNewline(const std::string &input);

        /**
         * @brief Remove color coding from string
         *
         * @param input string to proceed
         * @return sanitized string
         */
        static std::string RemoveColorCoding(const std::string &input);

        /**
         * @brief Joins a string array to a string using the given delimiter.
         *
         * @param vec vector of strings
         * @param delimiter delimiter
         * @param startIndex starting index default = 0
         * @return joined string
         */
        static std::string Join(const std::vector<std::string> &vec, const std::string &delimiter = " ", int startIndex = 0);

        /**
         * @brief Strip whitespaces
         *
         * @param s string to split
         * @return string having all whitespaces removed
         */
        static std::string StripWhiteSpaces(std::string &s);

        /**
         * @brief Strip line endings
         *
         * @param s string to split
         * @return string having all line endings removed
         */
        static std::string StripLineEndings(std::basic_string<char, std::char_traits<char>, std::allocator<char> > s);

        /**
         * @brief Strip beginning of string
         *
         * @param s1 string to strip
         * @param s2 substring to strip
         * @return string with beginning
         */
        static std::string StripBeginning(const std::string &s1, const std::string &s2);

        /**
         * @brief Return true if s1 and s2 are equal.
         *
         * @param s1 first input string
         * @param s2 second input string
         * @return true if s11 and s2 are equal
         */
        static bool Equals(const std::string &s1, const std::string &s2);

        /**
         * @brief Return true if s1 and s2 are not case-sensitive equal.
         *
         * @param s1 first input string
         * @param s2 second input string
         * @return true if s11 and s2 are equal
         */
        static bool EqualsIgnoreCase(const std::string &s1, const std::string &s2);

        /**
         * @brief Returns true in case the string is either null or empty
         *
         * @param s1 pointer to the string to check
         * @return true if the string is null or empty.
         */
        static bool IsNullOrEmpty(const std::string *s1);

        /**
         * @brief Returns true in case the string s1 contains string s1
         *
         * @param s1 string to check
         * @param s2 probe string
         * @return true if the string s1 contains string s2.
         */
        static bool Contains(const std::string &s1, const std::string &s2);

        /**
         * @brief Returns true in case the string s1 contains string s1, both string are compares case insensitive
         *
         * @param s1 string to check
         * @param s2 probe string
         * @return true if the string s1 contains string s2.
         */
        static bool ContainsIgnoreCase(const std::string &s1, const std::string &s2);

        /**
         * @brief Check whether the given string starts with the given prefix
         *
         * @param s1 string to check
         * @param s2 probe string
         * @return true if the string s1 starts with s2.
         */
        static bool StartsWith(const std::string &s1, const std::string &s2);

        /**
         * @brief Check whether the given string starts with the given prefix, ignoring the case
         *
         * Both string are converted to lowercase before compared.
         *
         * @param s1 string to check
         * @param s2 probe string
         * @return true if the string s1 starts with s2.
         */
        static bool StartsWithIgnoringCase(const std::string &s1, const std::string &s2);

        /**
         * @brief Check whether the given string StartServer with the given suffix
         *
         * @param s1 string to check
         * @param s2 probe string
         * @return true if the string s1 ends with s2.
         */
        static bool EndsWith(const std::string &s1, const std::string &s2);

        /**
         * @brief Returns a substring by index.
         *
         * @param string string to process
         * @param beginIndex begin index
         * @param endIndex end index
         * @return substring
         */
        static std::string SubString(const std::string &string, int beginIndex, int endIndex);

        /**
         * @brief Returns a substring by from 0 until a certain character.
         *
         * @param string string to process
         * @param delimiter delimiter character
         * @return substring
         */
        static std::string SubStringUntil(const std::string &string, const std::string &delimiter);

        /**
         * @brief Return the substring starting at delimiter.
         *
         * @param string string to test
         * @param delimiter delimiter
         * @return substring after delimiter
         */
        static std::string SubStringAfter(const std::string &string, const std::string &delimiter);

        /**
         * @brief Replace in C-string.
         *
         * @param target original
         * @param pattern pattern
         * @param replacement replacement string
         */
        static void Replace(std::string &target, const std::string &pattern, const std::string &replacement);

        /**
         * @brief Returns a stripped string with continuation chars (...)
         *
         * @param input original string
         * @param length maximal length
         * @return shorten string with continuation characters
         */
        static std::string Continuation(const std::string &input, int length);

        /**
         * @brief Left trim a string.
         *
         * @param s input string
         * @return trimmed string
         */
        static std::string Ltrim(std::string s) {
            s.erase(s.find_last_not_of(' ') + 1);
            return s;
        }

        /**
         * @brief Right trim a string.
         *
         * @param s input string
         * @return trimmed string
         */
        static std::string Rtrim(std::string s) {
            s.erase(0, s.find_first_not_of(' '));
            return s;
        }

        /**
         * @brief Trim a string.
         *
         * @param s input string
         * @return trimmed string
         */
        static std::string Trim(std::basic_string<char> s) {
            s = Rtrim(s);
            s = Ltrim(s);
            return s;
        }

        /**
         * @brief Returns the string in URL encoding format.
         *
         * @param input input string
         * @return URL encoded string.
         */
        static std::string UrlEncode(const std::string &input);

        /**
         * @brief Returns a decoded string as plain text.
         *
         * @param input input string
         * @return URL decoded string.
         */
        static std::string UrlDecode(const std::string &input);

        /**
         * @brief Returns an escaped string.
         *
         * @param input input string
         * @return escaped string.
         */
        static std::string Quoted(const std::string &input);

        /**
         * @brief Convert the given string to a hex encoded string.
         *
         * @param input input byte array
         * @param length length of the byte array
         * @return hex encoded string
         */
        static std::string ToHexString(const unsigned char *input, size_t length);

        /**
         * @brief Sanitizes the string and removes Convert the given string to a hex encoded string.
         *
         * @param input input byte array
         * @return hex encoded string
         */
        static std::string SanitizeUtf8(std::string &input);

        /**
         * @brief Strip any chunk signatures from the input string
         *
         * @param input input string
         * @return stripped input string
         */
        static std::string StripChunkSignature(const std::string &input);

        /**
         * @brief Converts a bool variable to a string.
         *
         * @param value bool value
         * @return string representation
         */
        static std::string ToString(bool value);

        /**
         * @brief Convert the string to snake case
         *
         * @param in in string
         * @return string in snake case
         */
        static std::string ToSnakeCase(const std::string &in);

        /**
         * @brief Return lower case string
         *
         * @param input input string
         * @return lower case string
         */
        static std::string ToLower(const std::string &input);

        /**
         * @brief Return upper case string
         *
         * @param input input string
         * @return upper case string
         */
        static std::string ToUpper(const std::string &input);

#ifdef _WIN32
        /**
         * @brief Convert from narrow string to wide string
         *
         * @param input narrow string input
         * @retur wide string
         */
        static std::wstring ConvertToWideString(const std::string &input);

        /**
         * @brief Convert from narrow string to wide string
         *
         * @param input narrow string input
         * @retur wide string
         */
        static std::string ConvertToNarrowString(const std::wstring &input);
#endif

    };

    inline std::string StringUtils::ToLower(const std::string &input) {
        return boost::algorithm::to_lower_copy(input);
    }

    inline std::string StringUtils::ToUpper(const std::string &input) {
        return boost::algorithm::to_upper_copy(input);
    }

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_STRING_UTILS_H
