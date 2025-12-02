//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_NOT_FOUND_EXCEPTION_H
#define AWSMOCK_CORE_NOT_FOUND_EXCEPTION_H

// Boost includes
#include <boost/beast/http.hpp>

// AwsMock includes

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Resource not found exception class.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class NotFoundException final : public std::exception {

      public:

        /**
         * @brief Constructor.
         *
         * @param code exception code, default: 0
         * @param resource exception resource
         * @param requestId exception resource ID
         */
        explicit NotFoundException(http::status code = http::status::not_found, const char *resource = nullptr, const char *requestId = nullptr);

        /**
         * @brief Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         * @param resource exception resource
         * @param requestId exception resource ID
         */
        explicit NotFoundException(const std::string &msg, http::status code = http::status::not_found, const char *resource = nullptr, const char *requestId = nullptr);

        /**
         * @brief Copy constructor.
         *
         * @param exc parent exception.
         */
        NotFoundException(const NotFoundException &exc);

        /**
         * @brief Destructor
         */
        ~NotFoundException() noexcept override;

        /**
         * @brief Assigment operator.
         */
        NotFoundException &operator=(const NotFoundException &exc);

        /**
         * @brief Returns the exception message.
         */
        [[nodiscard]] std::string message() const noexcept;

        /**
         * @brief Returns the exception message.
         */
        [[nodiscard]] http::status code() const noexcept;

        /**
         * @brief Returns the exception resource.
         */
        [[nodiscard]] const char *resource() const noexcept;

        /**
         * @brief Returns the exception request ID.
         */
        [[nodiscard]] const char *requestId() const noexcept;

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

        /**
         * Resource
         */
        const char *_resource;

        /**
         * Request ID
         */
        const char *_requestId;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_NOT_FOUND_EXCEPTION_H
