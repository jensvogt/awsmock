//
// Created by vogje01 on 30/10/2023.
//

#ifndef AWSMOCK_CORE_NUMBER_UTILS_H
#define AWSMOCK_CORE_NUMBER_UTILS_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/lexical_cast.hpp>

// AwsMock includes

namespace AwsMock::Core {

    /**
     * @brief Number utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class NumberUtils {

      public:

        /**
         * @brief Return a string with the value in kilobytes.
         *
         * @param value value to print
         * @return value string
         */
        static std::string ToKilobytesStr(long value);

        /**
         * @brief Converts the given number to kilobytes.
         *
         * @param value value to convert
         * @return value in kilobytes
         */
        static long ToKilobytes(long value);

        /**
         * @brief Return a string with the value in megabytes.
         *
         * @param value value to print
         * @return value string
         */
        static std::string ToMegabytesStr(long value);

        /**
         * @brief Converts the given number to megabytes.
         *
         * @param value value to convert
         * @return value in kilobytes
         */
        static long ToMegabytes(long value);

        /**
         * @brief Returns a byte array from an integer
         *
         * @param n integer value
         * @param bytes output bytes
         * @param offset offset
         */
        static void GetIntAsByteArray(int n, unsigned char *bytes, int offset);

        /**
         * @brief Returns a integer from a hex string
         *
         * @param hex hex string
         * @return converted integer
         */
        static int HexToInt(const std::string &hex);

        /**
         * @brief Returns an integer from a string
         *
         * @par
         * Returns -1 in case str is empty.
         *
         * @param str string
         * @return converted integer
         */
        static int ToInt(const std::string &str);

        /**
         * @brief Returns a double from a string
         *
         * @par
         * Returns -1 in case str is empty.
         *
         * @param str string
         * @return converted double
         */
        static double ToDouble(const std::string &str);
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_NUMBER_UTILS_H
