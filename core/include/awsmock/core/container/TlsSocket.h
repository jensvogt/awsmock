//
// Created by vogje01 on 5/25/26.
//

#pragma once

// C++ includes
#include <map>
#include <string>

// Boost includes
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

// AwsMock includes
#include <awsmock/core/container/DomainSocket.h>
#include <awsmock/core/container/DomainSocketResult.h>
#include <awsmock/core/logging/LogStream.h>

namespace Awsmock::Core {

    namespace http = boost::beast::http;

    /**
     * @brief Send commands over a TLS/HTTPS connection to the Docker daemon.
     *
     * @par
     * Connects to the Docker daemon via HTTPS with mutual TLS authentication.
     * The Docker daemon must be configured to listen on a TCP port with TLS enabled
     * (typically tcp://0.0.0.0:2376). The client must supply a CA certificate, a
     * client certificate, and a client private key.
     *
     * @par
     * Configure Docker for TLS by adding the following to the daemon configuration:
     * @code{.yaml}
     * awsmock:
     *   docker:
     *     tls:
     *       enabled: true
     *       host: localhost
     *       port: 2376
     *       ca-cert: /path/to/ca.pem
     *       cert: /path/to/cert.pem
     *       key: /path/to/key.pem
     *       verify-peer: true
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TlsSocket final : public DomainSocket {

      public:

        /**
         * @brief Constructor
         *
         * @param host Docker daemon hostname
         * @param port Docker daemon TLS port (typically 2376)
         * @param caFile path to CA certificate (PEM format)
         * @param certFile path to client certificate (PEM format)
         * @param keyFile path to client private key (PEM format)
         * @param verifyPeer whether to verify the server certificate (default: true)
         */
        TlsSocket(const std::string &host, int port, const std::string &caFile, const std::string &certFile, const std::string &keyFile, bool verifyPeer = true);

        /**
         * @brief Send JSON data
         *
         * @param method HTTP method
         * @param path URL path
         * @return result struct
         */
        DomainSocketResult SendJson(verb method, const std::string &path) override;

        /**
         * @brief Send JSON data
         *
         * @param method HTTP method
         * @param path URL path
         * @param body optional HTTP body
         * @return result struct
         */
        DomainSocketResult SendJson(verb method, const std::string &path, const std::string &body) override;

        /**
         * @brief Send JSON data
         *
         * @param method HTTP method
         * @param path URL path
         * @param body optional HTTP body
         * @param headers optional HTTP headers
         * @return result struct
         */
        DomainSocketResult SendJson(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) override;

        /**
         * @brief Send binary data
         *
         * @param method HTTP method
         * @param path URL path
         * @param fileName filename to send
         * @return result struct
         */
        DomainSocketResult SendBinary(verb method, const std::string &path, const std::string &fileName) override;

        /**
         * @brief Send binary data
         *
         * @param method HTTP method
         * @param path URL path
         * @param fileName filename to send
         * @param headers optional HTTP headers
         * @return result struct
         */
        DomainSocketResult SendBinary(verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) override;

        /**
         * @brief Send an attach-container command
         *
         * @par
         * Note: TLS attach streams over a separate TCP channel. The returned UNIX socket is unused;
         * log lines are forwarded directly to the supplied WebSocket.
         *
         * @param method HTTP method
         * @param path URL path
         * @param headers optional HTTP headers
         * @param ws web socket
         * @return unused placeholder socket
         */
        [[nodiscard]]
        boost::asio::local::stream_protocol::socket SendAttach(verb method, const std::string &path, const std::map<std::string, std::string> &headers, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws) override;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Core"};


        /**
         * @brief Create a connected and handshaked TLS stream
         *
         * @param ctx io_context to use
         * @param ec error code output
         * @return TLS stream
         */
        [[nodiscard]]
        boost::beast::ssl_stream<boost::beast::tcp_stream> Connect(boost::asio::io_context &ctx, boost::system::error_code &ec) const;

        /**
         * Docker daemon hostname
         */
        std::string _tlsHost;

        /**
         * Docker daemon TLS port
         */
        int _tlsPort;

        /**
         * Shared SSL context (holds loaded certificates)
         */
        boost::asio::ssl::context _sslContext;
    };

}// namespace Awsmock::Core
