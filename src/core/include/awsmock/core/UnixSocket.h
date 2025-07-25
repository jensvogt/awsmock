//
// Created by vogje01 on 5/28/24.
//

#ifndef AWSMOCK_CORE_UNIX_SOCKET_H
#define AWSMOCK_CORE_UNIX_SOCKET_H

// C++ includes
#include <map>
#include <regex>
#include <string>

// Boost includes
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/DomainSocket.h>
#include <awsmock/core/DomainSocketResult.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Send command over the Unix/MacOS TCP socker to the docker daemon on /var/run/docker.sock
     *
     * @par
     * The Unix/MacOS URL ist normally /var/run/docker.sock. If you run the AwsMock manager as non.root daemon
     * make sure the AwsMock user has access to the docker daemon REST API. This can be normally done using an
     * entry in the docker group on Unix/MacOS systems.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class UnixSocket : public DomainSocket {

      public:

        /**
         * @brief Constructor
         *
         * @param path domain socket path
         */
        explicit UnixSocket(const std::string &path) : DomainSocket(path) {};

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
        DomainSocketResult SendJson(verb method, const std::string &path) override;

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
        DomainSocketResult SendJson(verb method, const std::string &path, const std::string &body) override;

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
        DomainSocketResult SendJson(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) override;

        /**
         * @brief Send binary data
         *
         * This will send a file as a boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
         * to boost http response.
         *
         * @param method HTTP method
         * @param path URL path
         * @param fileName filename to send
         * @return result struct
         * @see Core::DomainSocketResult
         */
        DomainSocketResult SendBinary(verb method, const std::string &path, const std::string &fileName) override;

        /**
         * @brief Send binary data
         *
         * This will send a file as a boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
         * to boost http response.
         *
         * @param method HTTP method
         * @param path URL path
         * @param fileName filename to send
         * @param headers optional HTTP headers
         * @return result struct
         * @see Core::DomainSocketResult
         */
        DomainSocketResult SendBinary(verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) override;

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
        [[nodiscard]] boost::asio::local::stream_protocol::socket SendAttach(verb method, const std::string &path, const std::map<std::string, std::string> &headers, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws) override;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_UNIX_SOCKET_H
