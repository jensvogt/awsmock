//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_FORBIDDEN_H
#define AWSMOCK_CORE_FORBIDDEN_H

// Boost includes
#include <boost/beast/http.hpp>
#include <boost/beast/http/status.hpp>

// AwsMock includes

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Resource not found exception class.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ForbiddenException final : public std::exception {

      public:

        /**
         * @brief Constructor.
         *
         * @param code exception code, default: 0
         */
        explicit ForbiddenException(http::status code = http::status::forbidden);

        /**
         * @brief Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         */
        explicit ForbiddenException(const std::string &msg, http::status code = http::status::forbidden);

        /**
         * @brief Copy constructor.
         *
         * @param exc parent exception.
         */
        ForbiddenException(const ForbiddenException &exc);

        /**
         * @brief Destructor
         */
        ~ForbiddenException() noexcept override;

        /**
         * @brief Assigment operator.
         */
        ForbiddenException &operator=(const ForbiddenException &exc);

        /**
         * @brief Rethrows the exception.
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

      private:

        /**
         * Code
         */
        const http::status _code;

        /**
         * Message
         */
        std::string _message;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_FORBIDDEN_H
