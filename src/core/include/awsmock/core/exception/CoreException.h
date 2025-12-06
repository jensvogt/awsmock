//
// Created by vogje01 on 02/09/2022.
//

#ifndef AWS_MOCK_CORE_CORE_EXCEPTION_H
#define AWS_MOCK_CORE_CORE_EXCEPTION_H

// C++ includes
#include <string>
#include <utility>

// Boost includes
#include <boost/beast/http.hpp>


// AwsMock includes


namespace AwsMock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Exception class. In case of a COM request failure a COMException is thrown.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CoreException final : public std::exception {

      public:

        /**
         * Constructor.
         *
         * @param code exception code, default: 0
         */
        explicit CoreException(const boost::beast::http::status code = boost::beast::http::status::internal_server_error) : _code(code) {}

        /**
         * Constructor.
         *
         * @param msg exception message
         * @param code exception code, default: 0
         */
        explicit CoreException(std::string msg, const boost::beast::http::status code = boost::beast::http::status::internal_server_error) : _code(code), _message(std::move(msg)) {}

        /**
         * Copy constructor.
         *
         * @param exc parent exception.
         */
        CoreException(const CoreException &exc);

        /**
         * Destructor
         */
        ~CoreException() noexcept override;

        /**
         * Assigment operator.
         */
        CoreException &operator=(const CoreException &exc);

        /**
         * Return message
         */
        [[nodiscard]] std::string message() const { return _message; }

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
         * Return code
         */
        [[nodiscard]] http::status code() const { return _code; }

        /**
         * Http status code
         */
        http::status _code;

        /**
       * Http status code
       */
        std::string _message;
        ;
    };

}// namespace AwsMock::Core

#endif//AWS_MOCK_CORE_CORE_EXCEPTION_H
