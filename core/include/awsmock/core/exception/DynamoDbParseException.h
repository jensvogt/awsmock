//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_DYNAMODB_PARSE_EXCEPTION_H
#define AWSMOCK_CORE_DYNAMODB_PARSE_EXCEPTION_H

// C*+ includes
#include <sstream>
#include <string>

// C++ includes
#include <exception>

// Boost includes
#include <boost/beast/http/status.hpp>
#include <utility>

// AwsMock includes

namespace AwsMock::Core {

    /**
     * @brief Bad request exception class.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbParseException final : public std::exception {

      public:

        /**
         * Constructor.
         *
         * @param code exception code, default: 0
         */
        explicit DynamoDbParseException(const boost::beast::http::status code = boost::beast::http::status::bad_request) : _code(code) {}

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         */
        explicit DynamoDbParseException(std::string msg, const boost::beast::http::status code = boost::beast::http::status::bad_request) : _code(code), _message(std::move(msg)) {}

        /**
         * Copy constructor.
         *
         * @param exc parent exception.
         */
        DynamoDbParseException(const DynamoDbParseException &exc);

        /**
         * Destructor
         */
        ~DynamoDbParseException() noexcept override;

        /**
         * Assigment operator.
         */
        DynamoDbParseException &operator=(const DynamoDbParseException &exc);

        /**
         * Rethrows the exception.
         */
        void rethrow() const;

        /**
         * @brief Overrides the std::exception message
         *
         * @return std::exception what
         */
        [[nodiscard]] const char *what() const noexcept override {
            return _message.c_str();
        }

        /**
         * @brief Overrides the std::exception message
         *
         * @return std::exception what
         */
        [[nodiscard]] const char *message() const noexcept {
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

#endif// AWSMOCK_CORE_DYNAMODB_PARSE_EXCEPTION_H
