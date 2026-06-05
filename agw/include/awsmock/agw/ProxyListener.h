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
    namespace net   = boost::asio;
    using tcp       = net::ip::tcp;

    /**
     * @brief Accepts incoming TCP connections and creates a ProxySession for each one.
     */
    class ProxyListener : public std::enable_shared_from_this<ProxyListener> {
      public:

        ProxyListener(net::io_context &ioc, const Dto::ApiGateway::ProxyConfig &cfg);

        void Run();

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

        net::io_context                       &_ioc;
        tcp::acceptor                          _acceptor;
        const Dto::ApiGateway::ProxyConfig    &_cfg;
    };

}// namespace Awsmock::Agw
