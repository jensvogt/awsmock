
//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewayListener.h>

namespace AwsMock::Service {

    GatewayListener::GatewayListener(boost::asio::io_context &ioc, const ip::tcp::endpoint &endpoint) : _ioc(ioc), _acceptor(make_strand(ioc)) {

        boost::beast::error_code ec;

        // Open the acceptor
        ec = _acceptor.open(endpoint.protocol(), ec);
        if (ec) {
            log_error << ec.message();
            return;
        }

        // Allow address reuse
        ec = _acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            log_error << ec.message();
            return;
        }

        // Bind to the server address
        ec = _acceptor.bind(endpoint, ec);
        if (ec) {
            log_error << ec.message();
            return;
        }

        // Start listening for connections
        ec = _acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            log_error << ec.message();
            return;
        }
    }

    void GatewayListener::Run() {
        dispatch(_acceptor.get_executor(), boost::beast::bind_front_handler(&GatewayListener::DoAccept, shared_from_this()));
    }

    void GatewayListener::DoAccept() {
        _acceptor.async_accept(make_strand(_ioc), boost::beast::bind_front_handler(&GatewayListener::OnAccept, shared_from_this()));
    }

    void GatewayListener::OnAccept(const boost::beast::error_code &ec, ip::tcp::socket socket) {
        if (ec) {
            log_error << ec.message();
        } else {
            // Create the http session and run it
            std::make_shared<GatewaySession>(_ioc, std::move(socket))->Run();
        }

        // Accept another connection
        DoAccept();
    }

}// namespace AwsMock::Service
