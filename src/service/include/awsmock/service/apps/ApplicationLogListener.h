//
// Created by vogje01 on 7/19/25.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_LOG_LISTENER_H
#define AWSMOCK_SERVICE_APPLICATION_LOG_LISTENER_H

// C++ includes
#include <memory>

// Boost includes
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/apps/ApplicationLogSession.h>

namespace AwsMock::Service {

    /**
     * @brief Accepts incoming connections and launches the sessions
     */
    class ApplicationLogListener : public std::enable_shared_from_this<ApplicationLogListener> {
        boost::asio::io_context &ioc_;
        boost::asio::ip::tcp::acceptor acceptor_;

      public:

        ApplicationLogListener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint) : ioc_(ioc), acceptor_(ioc) {

            boost::beast::error_code ec;

            // Open the acceptor
            acceptor_.open(endpoint.protocol(), ec);
            if (ec) {
                log_error << "Open failed, errorCode: " << ec.message() << std::endl;
                return;
            }

            // Allow address reuse
            acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
            if (ec) {
                log_error << "SetOptions failed, errorCode: " << ec.message() << std::endl;
                return;
            }

            // Bind to the server address
            acceptor_.bind(endpoint, ec);
            if (ec) {
                log_error << "Bind failed, errorCode: " << ec.message() << std::endl;
                return;
            }

            // Start listening for connections
            acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error << "Listen failed, errorCode: " << ec.message() << std::endl;
                return;
            }
        }

        /**
         * Start accepting incoming connections
         */
        void run() {
            do_accept();
        }

      private:

        void do_accept() {
            // The new connection gets its own strand
            acceptor_.async_accept(boost::asio::make_strand(ioc_), boost::beast::bind_front_handler(&ApplicationLogListener::on_accept, shared_from_this()));
        }

        void on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket) {
            if (ec) {
                log_error << "Accept failed, errorCode: " << ec.message() << std::endl;
            } else {
                // Create the session and run it
                std::make_shared<ApplicationLogSession>(std::move(socket))->Run();
            }

            // Accept another connection
            do_accept();
        }
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_LOG_LISTENER_H
