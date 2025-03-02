//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_BAD_REQUEST_EXCEPTION_H
#define AWSMOCK_CORE_BAD_REQUEST_EXCEPTION_H

// C*+ includes
#include <sstream>
#include <string>

// C++ includes
#include <exception>

// Boost includes
#include <boost/beast/http/status.hpp>
#include <utility>

// AwsMock includes
#include <awsmock/core/Macros.h>

namespace AwsMock::Core {

    /**
     * @brief Bad request exception class.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class BadRequestException final : public std::exception {

      public:

        /**
         * Constructor.
         *
         * @param code exception code, default: 0
         */
        AWSMOCK_API explicit BadRequestException(const boost::beast::http::status code = boost::beast::http::status::bad_request) : _code(code) {}

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         */
        AWSMOCK_API explicit BadRequestException(std::string msg, const boost::beast::http::status code = boost::beast::http::status::bad_request) : _code(code), _message(std::move(msg)) {}

        /**
         * Copy constructor.
         *
         * @param exc parent exception.
         */
        AWSMOCK_API BadRequestException(const BadRequestException &exc);

        /**
         * Destructor
         */
        AWSMOCK_API ~BadRequestException() noexcept override;

        /**
         * Assigment operator.
         */
        AWSMOCK_API BadRequestException &operator=(const BadRequestException &exc);

        /**
         * Rethrows the exception.
         */
        AWSMOCK_API void rethrow() const;

        /**
         * @brief Overrides the std::exception message
         *
         * @return std::exception what
         */
        AWSMOCK_API [[nodiscard]] const char *what() const noexcept override {
            return _message.c_str();
        }

      private:

        /**
         * HTTP status code
         */
        boost::beast::http::status _code;

        /**
         * Exception message
         */
        std::string _message;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_BAD_REQUEST_EXCEPTION_H
