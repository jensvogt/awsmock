//
// Created by vogje01 on 5/28/24.
//

#ifndef AWSMOCK_CORE_HTTP_SOCKET_H
#define AWSMOCK_CORE_HTTP_SOCKET_H

// C++ includes
#include <map>
#include <regex>
#include <string>

// Boost includes
#include <boost/beast.hpp>
#include <boost/exception/diagnostic_information.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    class HttpSocket {

      public:

        /**
         * @brief Constructor
         */
        explicit HttpSocket() = default;

        /**
         * @brief Send a JSON string to an HTTP endpoint
         *
         * @param method HTTP method
         * @param host HTTP host
         * @param port HTTP port
         * @param path URL path
         * @param body HTTP body
         * @param headers HTTP headers
         * @return HTTP response
         */
        static HttpSocketResponse SendJson(http::verb method, const std::string &host, int port, const std::string &path, const std::string &body = {}, const std::map<std::string, std::string> &headers = {});

        /**
         * @brief Send a JSON string to an HTTP endpoint
         *
         * @param method HTTP method
         * @param module HTTP method
         * @param host HTTP host
         * @param port HTTP port
         * @param path URL path
         * @param signedHeaders list of headers which should be included in the signature
         * @param body HTTP body
         * @param headers HTTP headers
         * @return HTTP response
         */
        static HttpSocketResponse SendAuthorizedJson(http::verb method, const std::string &module, const std::string &host, int port, const std::string &path, const std::string &signedHeaders, std::map<std::string, std::string> &headers, const std::string &body = {});

        /**
         * @brief Send a binary to an HTTP endpoint
         *
         * This is usually a file to send ay application/octet-stream.
         *
         * @param method HTTP method
         * @param host HTTP host
         * @param port HTTP port
         * @param path UTRL path
         * @param filename name of the file to send
         * @param headers HTTP headers
         * @return HTTP response
         */
        static HttpSocketResponse SendBinary(http::verb method, const std::string &host, int port, const std::string &path, const std::string &filename = {}, const std::map<std::string, std::string> &headers = {});

      private:

        /**
         * @brief Prepare an HTTP message
         *
         * @param method HTTP method
         * @param host host to connect to
         * @param path URL path
         * @param body HTTP body
         * @param headers HTTP headers
         */
        static http::request<http::string_body> PrepareJsonMessage(http::verb method, const std::string &host, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers);

        /**
         * @brief Prepare an HTTP message
         *
         * @param method HTTP method
         * @param path URL path
         * @param filename name of the file to send
         * @param headers HTTP headers
         */
        static http::request<http::file_body> PrepareBinaryMessage(http::verb method, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers);

        /**
         * @brief Prepare HTTP response
         *
         * @param response HTTP response
         * @return response struct
         * @see Core::DomainSocketResult
         */
        static HttpSocketResponse PrepareResult(http::response<http::string_body> response);

        /**
         * @brief Prepare HTTP response
         *
         * @param response HTTP response
         * @return response struct
         * @see Core::DomainSocketResult
         */
        static HttpSocketResponse PrepareResult(http::response<http::dynamic_body> response);

        /**
         * Domain socket path
         */
        std::string _path;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_HTTP_SOCKET_H
