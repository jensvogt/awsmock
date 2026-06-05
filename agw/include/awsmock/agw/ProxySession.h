#pragma once

// C++ includes
#include <memory>

// Boost includes
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/optional.hpp>

// Awsmock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/apigateway/model/ProxyConfig.h>

namespace Awsmock::Agw {

    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;

    /**
     * Handles the initialization and management of a proxy session, providing
     * control over its operations and communication settings.
     *
     * @param sessionId A distinct identifier associated with the session.
     * @param connectionConfig Configuration parameters defining the proxy's connection behavior.
     */
    class ProxySession : public std::enable_shared_from_this<ProxySession> {
      public:

        /**
         * Represents a proxy session that manages the lifecycle and operations
         * of a proxy connection in a networked environment.
         *
         * @param ioc The unique identifier of the proxy session.
         * @param socket The socket to connect to
         * @param cfg Proxy configuration
         * @return An instance of the ProxySession that maintains the state of the proxy connection.
         */
        ProxySession(net::io_context &ioc, tcp::socket &&socket, const Dto::ApiGateway::ProxyConfig &cfg);

        /**
         * Executes the specified task with the given parameters, managing its state
         * and ensuring proper resource handling during execution.
         *
         * @return Returns a boolean indicating the success or failure of the task execution.
         */
        void Run();

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGatewayProxy"};

        // ── client → agw ──────────────────────────────────────────────
        void DoRead();
        void OnRead(const beast::error_code &ec, std::size_t);

        // ── agw → backend ─────────────────────────────────────────────
        void OnResolve(const beast::error_code &ec, const tcp::resolver::results_type &results);
        void OnConnect(const beast::error_code &ec, tcp::endpoint);
        void OnForward(const beast::error_code &ec, std::size_t);

        // ── backend → agw ─────────────────────────────────────────────
        void OnBackendRead(const beast::error_code &ec, std::size_t);

        // ── agw → client ──────────────────────────────────────────────
        void OnClientWrite(bool keepAlive, const beast::error_code &ec, std::size_t);
        void DoClose();

        // ── helpers ──────────────────────────────────────────────────────
        void PrepareForwardRequest();
        void SendBadGateway(const std::string &detail);

        net::io_context &_ioc;
        beast::tcp_stream _clientStream;
        tcp::resolver _resolver;

        // Created fresh for each proxied request (per-request backend connection).
        boost::optional<beast::tcp_stream> _backendStream;

        boost::optional<http::request_parser<http::dynamic_body>> _reqParser;
        boost::optional<http::response_parser<http::dynamic_body>> _respParser;

        http::request<http::dynamic_body> _req;
        http::response<http::dynamic_body> _resp;

        beast::flat_buffer _clientBuffer;
        beast::flat_buffer _backendBuffer;

        const Dto::ApiGateway::ProxyConfig &_cfg;
    };

}// namespace Awsmock::Agw
