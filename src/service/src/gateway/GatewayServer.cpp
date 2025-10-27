//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/gateway/GatewayServer.h>

namespace AwsMock::Service {

    GatewayServer::GatewayServer(boost::asio::io_context &ios) : AbstractServer("gateway"), _ios(ios) {

        // Get HTTP configuration values
        _port = Core::Configuration::instance().GetValue<int>("awsmock.gateway.http.port");
        _address = Core::Configuration::instance().GetValue<std::string>("awsmock.gateway.http.address");
        log_debug << "Gateway server initialized";

        // Check module active
        if (!IsActive("gateway")) {
            log_info << "Gateway module inactive";
            return;
        }

        // Set running
        SetRunning();

        // Create and launch a listening port
        const auto listener = std::make_shared<GatewayListener>(_ios, ip::tcp::endpoint{ip::make_address(_address), _port});
        listener->Run();

        log_info << "Gateway server started, endpoint: " << _address << " port: " << _port;
    }

    void GatewayServer::Shutdown(){
        _ios.stop();
    }

}// namespace AwsMock::Service
