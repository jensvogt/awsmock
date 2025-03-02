//
// Created by vogje01 on 13/05/2023.
//

#ifndef AWSMOCK_CORE_UTILS_STREAMFILTER_H
#define AWSMOCK_CORE_UTILS_STREAMFILTER_H

// C++ standard includes
#include <iostream>
#include <streambuf>

// AwsMock includes

namespace AwsMock::Core {

    static const int invalidChars[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x0b, 0x0c, 0x0e, 0x0f,
                                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

    /**
     * @brief Input stream filter class.
     *
     * <p>Can be used with the standard filter classes as described in https://en.cppreference.com/w/cpp/header/cctype</p>
     * <p>Usage:
     * @code{.cpp}
     * std::stringstream input;
     * std::istream filtered;
     * filtered = XmlCharacterStreamFilter(input, filtered);
     * @endcode
     * or with a standard filter (filtering all digits)
     * @code{.cpp}
     * std::stringstream input;
     * std::istream filtered;
     * filtered = ApplyFilter(input, filtered, std::isdigit);
     * @endcode
     *
     * @see https://cplusplus.com/forum/general/173565/
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class StreamFilter {

      public:

        /**
         * @brief Filters all non-XML characters from the supplied input stream.
         *
         * <p>Non-XML characters are all characters < 0x20, except 0x09 (TAB), 0x0A (LF), 0x0D (CR)</p>
         *
         * @param sin input stream
         * @param sout output stream
         * @return output stream
         */
        static std::istream &XmlCharacterStreamFilter(std::iostream &sin, std::istream &sout) {
            return ApplyFilter(sin, sout, XmlCharacterFilter);
        }

        /**
         * @brief Filters all character which are not recognized by the given standard filter.
         *
         * <p> Standard filters are defined in &lt;cctype&gt; like std::isdigit, std::isalpha etc.</p>
         *
         * @param sin input stream
         * @param sout output stream
         * @param filter filter function
         * @return output stream
         * @see https://en.cppreference.com/w/cpp/header/cctype
         */
        static std::istream &ApplyFilter(std::iostream &sin, std::istream &sout, int (*filter)(int)) {
            char c;
            while (sin.get(c))
                if (!filter(c))
                    sout.rdbuf()->sputc(c);
            sin.clear();
            sout.clear();
            return sout;
        }

        /**
         * @brief Filters all character which are not recognized by the given standard filter.
         *
         * <p> Standard filters are defined in &lt;cctype&gt; like std::isdigit, std::isalpha etc.</p>
         *
         * @param sin input stream
         * @param sout output stream
         * @param filter filter function
         * @return output stream
         * @see https://en.cppreference.com/w/cpp/header/cctype
         */
        static std::ostream &ApplyFilter(std::istream &sin, std::ostream &sout, int (*filter)(int)) {
            char c;
            while (sin.get(c))
                if (filter(c))
                    sout.put(c);
            return sout;
        }

        /**
         * @brief Filters all character which are not recognized by the given standard filter.
         *
         * <p> Standard filters are defined in &lt;cctype&gt; like std::isdigit, std::isalpha etc.</p>
         *
         * @param sin input file stream
         * @param sout output stream
         * @param filter filter function
         * @return output stream
         * @see https://en.cppreference.com/w/cpp/header/cctype
         */
        static std::ostream &ApplyFilter(std::ifstream &sin, std::ostream &sout, int (*filter)(int)) {
            char c;
            while (sin.get(c))
                if (filter(c))
                    sout.put(c);
            return sout;
        }

      private:

        /**
         * @brief Non-XML character filter.
         *
         * <p>Non-XML characters are all characters < 0x20, except 0x09 (TAB), 0x0A (LF), 0x0D (CR)</p>
         *
         * @param c input character
         * @return 1 (true) if c is valid, oherwise false
         */
        static int XmlCharacterFilter(int c) {
            return std::find(std::begin(invalidChars), std::end(invalidChars), c) != std::end(invalidChars);
        }
    };

}// namespace AwsMock::Core

#endif//AWSMOCK_CORE_UTILS_STREAMFILTER_H
