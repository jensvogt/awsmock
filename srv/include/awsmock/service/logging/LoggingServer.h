//
// Created by vogje01 on 2/5/26.
//

#ifndef AWSMOCK_SERVICE_LOGGING_SERVER_H
#define AWSMOCK_SERVICE_LOGGING_SERVER_H

// C++ includes
#include <mutex>
#include <set>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>

namespace AwsMock::Service::Logging {
    class LoggingServer {

      public:

        /**
         * @brief Run the server
         *
         * @param ioc boost IO context
         * @param port port
         * @param mgr manager
         */
        void Run(boost::asio::io_context &ioc, unsigned short port, std::shared_ptr<websocket_session_manager> mgr) {
            boost::asio::ip::tcp::acceptor acceptor{ioc, {boost::asio::ip::tcp::v4(), port}};
            for (;;) {
                boost::asio::ip::tcp::socket socket{ioc};
                acceptor.accept(socket);

                auto ws = std::make_shared<boost::beast::websocket::stream<boost::beast::tcp_stream>>(std::move(socket));
                ws->accept();
                mgr->add(ws);
            }
        }
    };

}// namespace AwsMock::Service::Logging

#endif// AWSMOCK_SERVICE_LOGGING_SERVER_H
