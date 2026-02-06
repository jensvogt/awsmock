//
// Created by vogje01 on 2/5/26.
//

#ifndef AWSMOCK_SERVICE_LOGGING_SERVER_H
#define AWSMOCK_SERVICE_LOGGING_SERVER_H

// C++ includes
#include <mutex>
#include <set>

// Boost includes
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace AwsMock::Service::Logging {
    /**
     * Manages all active WebSocket connections
     */
    class WebSocketSessionManager {

        std::set<std::shared_ptr<websocket::stream<beast::tcp_stream>>> sessions;
        std::mutex mutex;

      public:

        void add(const std::shared_ptr<websocket::stream<beast::tcp_stream>> &session) {
            std::lock_guard lock(mutex);
            sessions.insert(session);
        }

        void broadcast(std::string const &message) {
            std::lock_guard lock(mutex);
            for (auto it = sessions.begin(); it != sessions.end();) {
                try {
                    (*it)->text(true);
                    (*it)->write(net::buffer(message));
                    ++it;
                } catch (...) {
                    it = sessions.erase(it);// Remove dead connections
                }
            }
        }
    };

    // The Boost.Log Backend
    class WebSocketSinkBackend : public boost::log::sinks::basic_formatted_sink_backend<char> {
        std::shared_ptr<WebSocketSessionManager> manager;

      public:

        explicit WebSocketSinkBackend(std::shared_ptr<WebSocketSessionManager> mgr) : manager(std::move(std::move(mgr))) {}

        void consume(boost::log::record_view const &rec, std::string const &formatted_message) const {
            manager->broadcast(formatted_message);
        }
    };

    [[noreturn]] inline void RunLoggingWebSocketServer(net::io_context &ioc, unsigned short port, const std::shared_ptr<WebSocketSessionManager> &mgr) {
        tcp::acceptor acceptor{ioc, {tcp::v4(), port}};
        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            auto ws = std::make_shared<websocket::stream<beast::tcp_stream>>(std::move(socket));
            ws->accept();
            mgr->add(ws);
        }
    }

    class LoggingServer {

      public:

        /**
         * @brief Run the server
         *
         * @param ioc boost IO context
         * @param port port
         * @param mgr manager
         */
        [[noreturn]] static void Run(boost::asio::io_context &ioc, unsigned short port, const std::shared_ptr<WebSocketSessionManager> &mgr) {
            boost::asio::ip::tcp::acceptor acceptor{ioc, {boost::asio::ip::tcp::v4(), port}};
            for (;;) {
                boost::asio::ip::tcp::socket socket{ioc};
                acceptor.accept(socket);

                auto ws = std::make_shared<websocket::stream<beast::tcp_stream>>(std::move(socket));
                ws->accept();
                mgr->add(ws);
            }
        }
    };

}// namespace AwsMock::Service::Logging

#endif// AWSMOCK_SERVICE_LOGGING_SERVER_H
