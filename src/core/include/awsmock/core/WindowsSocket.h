//
// Created by vogje01 on 5/28/24.
//

#ifndef AWSMOCK_CORE_WINDOWS_SOCKET_H
#define AWSMOCK_CORE_WINDOWS_SOCKET_H

// C++ includes
#include <map>
#include <regex>
#include <string>

// Boost includes
#include <boost/beast.hpp>
#include <boost/regex/v5/regex.hpp>
#include <boost/regex/v5/regex_fwd.hpp>

// AwsMock includes
#include <awsmock/core/DomainSocket.h>
#include <awsmock/core/DomainSocketResult.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Send command over the Windows TCP socker to the docker daemon on localhost:2375.
     *
     * @par
     * The Windows URL ist normally tcp://localhost:2375. If you use another port, or a proxy, you can set the URL in the
     * AwsMock configuration file.
     *
     * @par
     * Currently the Windows port does not support TLS. Therefore, you need to switch off TLS in the docker daemon
     * configuration.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class WindowsSocket final : public DomainSocket {

      public:

        /**
         * @brief Constructor
         *
         * @param path domain socket path
         */
        explicit WindowsSocket(const std::string &path) : DomainSocket(path) {};

        /**
         * @brief Send JSON data
         *
         * @par
         * This will send a command as boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
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
         * This will send a JSON string as boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
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
         * This will send a file as boost http request to the domain socket and waits for the response. The call is synchronous and the response is converted
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

      private:

        /**
         * @brief Get the host and port from the supplied URL
         *
         * @param url HTTP URL to extract host and port from
         * @param host HTTP host
         * @param port HTTP port
         */
        static void GetHostPort(const std::string &url, std::string &host, int &port);
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_WINDOWS_SOCKET_H
