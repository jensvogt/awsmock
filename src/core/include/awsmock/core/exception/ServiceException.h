//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWSMOCK_CORE_SERVICE_EXCEPTION_H
#define AWSMOCK_CORE_SERVICE_EXCEPTION_H

// Boost includes
#include <boost/beast/http.hpp>

// AwsMock includes

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Service exception class. In case of a request failure a ServiceException is thrown.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ServiceException final : public std::exception {

      public:

        /**
         * @brief Constructor.
         *
         * @param code exception code, default: 0
         */
        explicit ServiceException(http::status code = http::status::internal_server_error);

        /**
         * @brief Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         */
        explicit ServiceException(const std::string &msg, http::status code = http::status::internal_server_error);

        /**
         * @brief Copy constructor.
         *
         * @param exc parent exception.
         */
        ServiceException(const ServiceException &exc);

        /**
         * @brief Destructor
         */
        ~ServiceException() noexcept override;

        /**
         * @brief Returns the exception message.
         */
        [[nodiscard]] std::string message() const noexcept;

        /**
         * @brief Returns the exception message.
         */
        [[nodiscard]] http::status code() const noexcept;

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

#endif//AWSMOCK_CORE_SERVICE_EXCEPTION_H
