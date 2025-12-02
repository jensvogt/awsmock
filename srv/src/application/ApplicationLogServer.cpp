//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/apps/ApplicationLogServer.h>

namespace AwsMock::Service {
    ApplicationLogServer::ApplicationLogServer() : AbstractServer("application-log") {

        const Core::Configuration &configuration = Core::Configuration::instance();
    }

    void ApplicationLogServer::operator()(const std::string &listenAddress, const long port) const {

        auto const address = boost::asio::ip::make_address(listenAddress);
        constexpr auto threads = std::max<int>(1, 5);

        // The io_context is required for all I/O
        boost::asio::io_context _ioc{5};

        // Create and launch a listening port
        std::make_shared<ApplicationLogListener>(_ioc, boost::asio::ip::tcp::endpoint{address, static_cast<unsigned short int>(port)})->run();

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i){
            v.emplace_back([&_ioc] { _ioc.run(); });
        }
        _ioc.run();
    }

    void ApplicationLogServer::Shutdown() {
        //_ioc.stop();
    }

}// namespace AwsMock::Service
