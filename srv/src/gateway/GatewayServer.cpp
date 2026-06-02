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

        // Create and launch a listening port, dual stack IPv4/v6
        const auto listener = std::make_shared<GatewayListener>(_ios, tcp::endpoint(tcp::v6(), _port));
        listener->Run();

        // Log endpoint
        log_info << "Gateway server started, endpoint: " << _address << " port: " << _port;
    }

    void GatewayServer::Shutdown() {
        WorkerPool().join();
        _ios.stop();
    }

}// namespace Awsmock::Service
