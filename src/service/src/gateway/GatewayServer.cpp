//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/gateway/GatewayServer.h>

namespace AwsMock::Service {

    GatewayServer::GatewayServer(boost::asio::io_context &ioc) : AbstractServer("gateway"), _ioc(ioc) {

        // Get HTTP configuration values
        Core::Configuration &configuration = Core::Configuration::instance();
        _port = configuration.getInt("awsmock.service.gateway.http.port", GATEWAY_DEFAULT_PORT);
        _host = configuration.getString("awsmock.service.gateway.http.host", GATEWAY_DEFAULT_HOST);
        _address = configuration.getString("awsmock.service.gateway.http.address", GATEWAY_DEFAULT_ADDRESS);
        _maxQueueLength = configuration.getInt("awsmock.service.gateway.http.max.queue", GATEWAY_MAX_QUEUE);
        _maxThreads = configuration.getInt("awsmock.service.gateway.http.max.threads", GATEWAY_MAX_THREADS);
        _requestTimeout = configuration.getInt("awsmock.service.gateway.http.timeout", GATEWAY_TIMEOUT);

        // Sleeping period
        _period = configuration.getInt("awsmock.worker.gateway.period", 10000);
        log_debug << "Gateway worker period: " << _period;

        // Create environment
        _region = configuration.getString("awsmock.region");
        log_debug << "GatewayServer initialized";
    }

    void GatewayServer::Initialize() {

        // Check module active
        if (!Database::ModuleDatabase::instance().IsActive("gateway")) {
            log_info << "Gateway module inactive";
            return;
        }
        log_info << "Gateway server starting";
    }

    void GatewayServer::Start() {
        boost::thread(&GatewayServer::Run, this).detach();
    }

    void GatewayServer::Run() {

        // Set running
        Database::ModuleDatabase::instance().SetState("gateway", Database::Entity::Module::ModuleState::RUNNING);

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        /*boost::asio::signal_set signals(_ioc, SIGINT, SIGTERM);
        signals.async_wait(
                [&](boost::beast::error_code const &, int) {
                    // Stop the `io_context`. This will cause `run()` to return immediately, eventually
                    // destroying the `io_context` and all the sockets in it.
                    _ioc.stop();
                    log_info << "Gateway stopped";
                });*/

        // Create and launch a listening port
        auto address = ip::make_address(_address);
        std::make_shared<GatewayListener>(_ioc, ip::tcp::endpoint{address, _port})->Run();

        // Run the I/O service on the requested number of threads
        _threads.reserve(_maxThreads - 1);
        for (auto i = _maxThreads - 1; i > 0; --i)
            _threads.emplace_back(
                    [this] {
                        _ioc.run();
                    });
        _ioc.run();
    }

    void GatewayServer::Shutdown() {
        _ioc.stop();
        for (auto &t: _threads) {
            t.join();
        }
    }

}// namespace AwsMock::Service
