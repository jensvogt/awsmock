//
// Created by vogje01 on 7/19/25.
//

#pragma once

// Boost includes
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>

// AwsMock include
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/container/ContainerService.h>

namespace Awsmock::Service {

    /**
     * @brief Streams Docker container logs to a WebSocket client.
     *
     * @par
     * The client connects with ws://host:port?containerId=<id>. The server
     * parses the containerId from the HTTP upgrade request and starts streaming
     * immediately — no handshake messages required.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationLogSession : public std::enable_shared_from_this<ApplicationLogSession> {

        boost::beast::websocket::stream<boost::beast::tcp_stream> _ws;
        boost::beast::flat_buffer _buffer;

      public:

        explicit ApplicationLogSession(boost::asio::ip::tcp::socket &&socket) : _ws(std::move(socket)) {}

        /**
         * @brief Read the HTTP upgrade request, parse containerId, accept the WebSocket,
         *        and start streaming Docker logs immediately.
         */
        void Run();
    };

}// namespace Awsmock::Service
