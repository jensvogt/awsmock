//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_JSON_EXCEPTION_H
#define AWSMOCK_CORE_JSON_EXCEPTION_H

// Boost includes
#include <boost/beast/http.hpp>

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief JSON exception class. In case of a JSON serialization/deserialization error.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class JsonException final : public std::exception {

      public:

        /**
         * Constructor.
         *
         * @param code exception code, default: 0
         * @param resource exception resource
         * @param requestId exception resource ID
         */
        explicit JsonException(http::status code = http::status::internal_server_error, const char *resource = nullptr, const char *requestId = nullptr);

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         * @param resource exception resource
         * @param requestId exception resource ID
         */
        explicit JsonException(const std::string &msg, http::status code = http::status::internal_server_error, const char *resource = nullptr, const char *requestId = nullptr);

        /**
         * Copy constructor.
         *
         * @param exc parent exception.
         */
        JsonException(const JsonException &exc);

        /**
         * Destructor
         */
        ~JsonException() noexcept override;

        /**
         * Assigment operator.
         */
        JsonException &operator=(const JsonException &exc);

        /**
         * Returns the exception message.
         */
        [[nodiscard]] std::string message() const noexcept;

        /**
         * Returns the exception message.
         */
        [[nodiscard]] http::status code() const noexcept;

        /**
         * Returns the exception resource.
         */
        [[nodiscard]] const char *resource() const noexcept;

        /**
         * Returns the exception request ID.
         */
        [[nodiscard]] const char *requestId() const noexcept;

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

#endif// AWSMOCK_CORE_JSON_EXCEPTION_H
