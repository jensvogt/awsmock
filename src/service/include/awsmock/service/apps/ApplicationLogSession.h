//
// Created by vogje01 on 7/19/25.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_LOG_SESSION_H
#define AWSMOCK_SERVICE_APPLICATION_LOG_SESSION_H

// Boost includes
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

// AwsMock include
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/apps/internal/WebSocketCommand.h>
#include <awsmock/service/container/ContainerService.h>

namespace AwsMock::Service {

    /**
     * @brief Logs all log messages to a websocket
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationLogSession : public std::enable_shared_from_this<ApplicationLogSession> {

        boost::beast::websocket::stream<boost::beast::tcp_stream> _ws;
        boost::beast::flat_buffer buffer_;

      public:

        /**
         * @brief Take ownership of the socket
         */
        explicit ApplicationLogSession(boost::asio::ip::tcp::socket &&socket) : _ws(std::move(socket)) {}

        /**
         * @brief Start the asynchronous operation
         */
        void Run();

        /**
         * @brief Accept an incoming message
         *
         * @param ec error code
         */
        void OnAccept(const boost::beast::error_code &ec);

        /**
         * @brief Read the message
         */
        void DoRead();

        /**
         * @brief Asynchronously read a message
         */
        void OnRead(const boost::beast::error_code &ec, std::size_t bytes_transferred);

        /**
         * @brief Asynchronously write a message
         */
        void OnWrite(const boost::beast::error_code &ec, std::size_t bytes_transferred);

        /**
         * @brief Handle websocket messages
         *
         * @param message websocket message
         * @param ws web socket
         */
        static std::string HandleEvent(const std::string &message, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws);

        /**
         * Websocket backend
         */
        boost::shared_ptr<Core::LogWebsocketSink> _backend;

        /**
         * Websocket sink
         */
        boost::shared_ptr<Core::webSocketSink_t> _sink;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_LOG_SESSION_H
