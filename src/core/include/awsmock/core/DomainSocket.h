//
// Created by vogje01 on 5/28/24.
//

#ifndef AWSMOCK_CORE_DOMAIN_SOCKET_H
#define AWSMOCK_CORE_DOMAIN_SOCKET_H

// C++ includes
#include <map>
#include <regex>
#include <string>

// Boost includes
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <utility>

// AwsMock includes
#include <awsmock/core/DomainSocketResult.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    class DomainSocket {

      public:

        virtual ~DomainSocket() = default;

        /**
         * @brief Constructor
         *
         * @param path domain socket path
         */
        explicit DomainSocket(std::string path) : _basePath(std::move(path)) {};

        /**
         * @brief Send JSON data
         *
         * @par
         * This will send a JSON string as a boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
         * to boost http response.
         *
         * @param method HTTP method
         * @param path URL path
         * @return result struct
         * @see Core::DomainSocketResult
         */
        [[nodiscard]] virtual DomainSocketResult SendJson(verb method, const std::string &path) = 0;

        /**
         * @brief Send JSON data
         *
         * @par
         * This will send a JSON string as a boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
         * to boost http response.
         *
         * @param method HTTP method
         * @param path URL path
         * @param body optional HTTP body
         * @return result struct
         * @see Core::DomainSocketResult
         */
        [[nodiscard]] virtual DomainSocketResult SendJson(verb method, const std::string &path, const std::string &body) = 0;

        /**
         * @brief Send JSON data
         *
         * @par
         * This will send a JSON string as a boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
         * to boost http response.
         *
         * @param method HTTP method
         * @param path URL path
         * @param body optional HTTP body
         * @param headers optional HTTP headers
         * @return result struct
         * @see Core::DomainSocketResult
         */
        [[nodiscard]] virtual DomainSocketResult SendJson(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) = 0;

        /**
         * @brief Send binary data
         *
         * This will send a file as boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
         * to boost http response.
         *
         * @param method HTTP method
         * @param path URL path
         * @param fileName filename to send
         * @return result struct
         * @see Core::DomainSocketResult
         */
        [[nodiscard]] virtual DomainSocketResult SendBinary(verb method, const std::string &path, const std::string &fileName) = 0;

        /**
         * @brief Send binary data
         *
         * This will send a file as boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
         * to boost http response.
         *
         * @param method HTTP method
         * @param path URL path
         * @param fileName filename to send
         * @param headers optional HTTP headers
         * @return result struct
         * @see Core::DomainSocketResult
         */
        [[nodiscard]] virtual DomainSocketResult SendBinary(verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) = 0;

        /**
         * @brief Send an attach-container command
         *
         * @param method HTTP method
         * @param path URL path
         * @param headers optional HTTP headers
         * @param ws web socket
         * @return result struct
         * @see Core::DomainSocketResult
         */
        [[nodiscard]] virtual boost::asio::local::stream_protocol::socket SendAttach(verb method, const std::string &path, const std::map<std::string, std::string> &headers, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws) = 0;

      protected:

        /**
         * @brief Prepare an HTTP message
         *
         * @param method HTTP method
         * @param path URL path
         * @param body HTTP body
         * @param headers HTTP headers
         */
        static request<string_body> PrepareJsonMessage(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers);

        /**
         * @brief Prepare an HTTP message
         *
         * @param method HTTP method
         * @param path URL path
         * @param filename name of the file to send
         * @param headers HTTP headers
         */
        static request<file_body> PrepareBinaryMessage(verb method, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers);

        /**
         * @brief Prepare HTTP message
         *
         * @param response HTTP response
         * @return response struct
         * @see Core::DomainSocketResult
         */
        static DomainSocketResult PrepareResult(response<string_body> response);

        /**
         * Domain socket path
         */
        std::string _basePath;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_DOMAIN_SOCKET_H
