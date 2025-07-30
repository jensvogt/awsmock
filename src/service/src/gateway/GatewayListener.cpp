
//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewayListener.h>

namespace AwsMock::Service {

    void GatewayListener::DoListen(boost::asio::io_context &ioc, ip::tcp::endpoint &endpoint, boost::asio::yield_context yield) {
        boost::beast::error_code ec;

        // Open the acceptor
        ip::tcp::acceptor acceptor(ioc);
        acceptor.open(endpoint.protocol(), ec);
        if (ec) {
            log_error << ec.message();
            return;
        }

        // Allow address reuse
        acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            log_error << ec.message();
            return;
        }

        // Bind to the server address
        acceptor.bind(endpoint, ec);
        if (ec) {
            log_error << ec.message();
            return;
        }

        // Start listening for connections
        acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            log_error << ec.message();
            return;
        }

        for (;;) {
            ip::tcp::socket socket(ioc);
            acceptor.async_accept(socket, yield[ec]);
            if (ec) {
                log_error << ec.message();
                return;
            }
            GatewaySession session;
            boost::asio::spawn(acceptor.get_executor(), std::bind(&GatewaySession::DoSession, session, boost::beast::tcp_stream(std::move(socket)), std::placeholders::_1), boost::asio::detached);
        }
    }

}// namespace AwsMock::Service
