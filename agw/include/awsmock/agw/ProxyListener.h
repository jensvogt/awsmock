#pragma once

// C++ includes
#include <memory>

// Boost includes
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>

// Awsmock includes
#include <awsmock/agw/ProxySession.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/apigateway/model/ProxyConfig.h>

namespace Awsmock::Agw {

    namespace beast = boost::beast;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;

    /**
     * @brief Manages the interaction layer between the client and the proxy server.
     */
    class ProxyListener : public std::enable_shared_from_this<ProxyListener> {
      public:

        /**
         * @brief Handles communication between a client and a proxy server
         *
         * @param ioc boost IO context
         * @param cfg configuration
         */
        ProxyListener(net::io_context &ioc, const Dto::ApiGateway::ProxyConfig &cfg);

        /**
         * @brief Represents a runnable task or process that can be initiated and executed.
         */
        void Run();

        /**
         * @brief Closes the acceptor and stops accepting new connections.
         */
        void Stop();

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGatewayProxy"};

        /**
         * @brief Handles the acceptance of incoming network connections asynchronously.
         */
        void DoAccept();

        /**
         * @brief Handles the acceptance of a new incoming connection.
         *
         * @param ec error structure
         * @param socket The incoming socket to be processed.
         */
        void OnAccept(beast::error_code ec, tcp::socket socket);

        /**
         * @brief Boost IO context
         */
        net::io_context &_ioc;

        /**
         * @brief Socket acceptor
         */
        tcp::acceptor _acceptor;

        /**
         * @brief Gateway configuration
         */
        Dto::ApiGateway::ProxyConfig _cfg;
    };

}// namespace Awsmock::Agw
