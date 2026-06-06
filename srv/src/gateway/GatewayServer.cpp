//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/gateway/GatewayServer.h>

namespace Awsmock::Service {

    GatewayServer::GatewayServer(boost::asio::io_context &ios) : AbstractServer("gateway"), _ios(ios) {

        // Get HTTP configuration values
        _port = Core::Configuration::instance().get<int>("awsmock.gateway.http.port");
        _address = Core::Configuration::instance().get<std::string>("awsmock.gateway.http.address");
        log_debug << "Gateway server initialized, address: " << _address << ", port: " << _port;

        // Check module active
        if (!IsActive("gateway")) {
            log_info << "Gateway module inactive";
            return;
        }

        // Set running
        SetRunning();

        // Create endpoint from the configured address so that an IPv4 address
        // (e.g. "0.0.0.0") binds an IPv4 socket rather than always forcing
        // dual-stack IPv6, which fails on Windows with WSAEACCES when the port
        // is in the OS-reserved range or the IPv4 side is already taken.
        boost::system::error_code addr_ec;
        const auto addr = net::ip::make_address(_address, addr_ec);
        const tcp::endpoint ep = addr_ec ? tcp::endpoint(tcp::v6(), _port) : tcp::endpoint(addr, _port);
        const auto listener = std::make_shared<GatewayListener>(_ios, ep);
        listener->Run();

        // Log endpoint
        log_info << "Gateway server started, endpoint: " << _address << " port: " << _port;
    }

    void GatewayServer::Shutdown() {
        WorkerPool().join();
        _ios.stop();
    }

}// namespace Awsmock::Service
