//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/gateway/GatewayServer.h>

namespace AwsMock::Service {

    GatewayServer::GatewayServer(boost::asio::io_context &ios) : AbstractServer("gateway"), _ios(ios) {

        // Get HTTP configuration values
        const Core::Configuration &configuration = Core::Configuration::instance();
        _port = configuration.GetValueInt("awsmock.gateway.http.port");
        _host = configuration.GetValueString("awsmock.gateway.http.host");
        _address = configuration.GetValueString("awsmock.gateway.http.address");
        log_debug << "Gateway server initialized";

        // Check module active
        if (!IsActive("gateway")) {
            log_info << "Gateway module inactive";
            return;
        }

        // Set running
        SetRunning();

        // Create and launch a listening port
        const auto address = ip::make_address(_address);
        std::make_shared<GatewayListener>(_ios, ip::tcp::endpoint{address, _port})->Run();
        log_info << "Gateway server started, endpoint: " << _address << " port: " << _port;
    }

    void GatewayServer::operator()() const {

        try {

            auto const address = ip::make_address(Core::Configuration::instance().GetValueString("awsmock.frontend.address"));
            unsigned short port = Core::Configuration::instance().GetValueInt("awsmock.frontend.port");
            std::string doc_root = Core::Configuration::instance().GetValueString("awsmock.frontend.doc-root");
            const int num_workers = Core::Configuration::instance().GetValueInt("awsmock.frontend.workers");

            boost::asio::io_context ioc{1};
            ip::tcp::acceptor acceptor{ioc, {address, port}};

            std::list<GatewayWorker> workers;
            for (int i = 0; i < num_workers; ++i) {
                workers.emplace_back(acceptor, doc_root);
                workers.back().Start();
            }
            log_info << "Gateway server started, endpoint: " << address << ":" << port << " workers: " << num_workers;
            ioc.run();

        } catch (const std::exception &e) {
            log_error << "Error: " << e.what() << std::endl;
        }
    }

}// namespace AwsMock::Service
