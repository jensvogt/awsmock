//
// Created by vogje01 on 12/17/24.
//

// AwsMock includes
#include <awsmock/service/frontend/HttpServer.h>

namespace AwsMock::Service::Frontend {

    void HttpServer::operator()() const {

        try {

            auto const address = net::ip::make_address(Core::Configuration::instance().GetValueString("awsmock.frontend.address"));
            unsigned short port = Core::Configuration::instance().GetValueInt("awsmock.frontend.port");
            std::string doc_root = Core::Configuration::instance().GetValueString("awsmock.frontend.doc-root");
            const int num_workers = Core::Configuration::instance().GetValueInt("awsmock.frontend.workers");

            boost::asio::io_context ioc{1};
            boost::asio::ip::tcp::acceptor acceptor{ioc, {address, port}};

            std::list<HttpWorker> workers;
            for (int i = 0; i < num_workers; ++i) {
                workers.emplace_back(acceptor, doc_root);
                workers.back().Start();
            }
            log_info << "Frontend server started, endpoint: " << address << ":" << port << " workers: " << num_workers;
            ioc.run();

        } catch (const std::exception &e) {
            log_error << "Error: " << e.what() << std::endl;
        }
    }

}// namespace AwsMock::Service::Frontend