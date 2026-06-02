//
// Created by vogje01 on 02/09/2022.
//

#pragma once

// C++ includes
#include <string>
#include <utility>

// Boost includes
#include <boost/beast/http.hpp>

namespace Awsmock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Container exception class.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ContainerException final : public std::exception {

      public:

        /**
         * Constructor.
         *
         * @param code exception code, default: 0
         */
        explicit ContainerException(const http::status code = http::status::internal_server_error) : _code(code) {
        }

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         */
        explicit ContainerException(std::string msg, const http::status code = http::status::internal_server_error) : _code(code), _message(std::move(msg)) {
        }

        /**
         * Copy constructor.
         *
         * @param exc parent exception.
         */
        ContainerException(const ContainerException &exc);

        /**
         * Destructor
         */
        ~ContainerException() noexcept override;

        /**
         * Assigment operator.
         */
        ContainerException &operator=(const ContainerException &exc);

        /**
         * Return message
         */
        [[nodiscard]]
        std::string message() const { return _message; }

        /**
         * @brief Overrides the std::exception message
         *
         * @return std::exception what
         */
        [[nodiscard]]
        const char *what() const noexcept override {
            return _message.c_str();
        }

      private:

        /**
         * Return code
         */
        [[nodiscard]]
        http::status code() const { return _code; }

        /**
         * Http status code
         */
        http::status _code;

        /**
       * Http status code
       */
        std::string _message;
    };

}// namespace Awsmock::Core
