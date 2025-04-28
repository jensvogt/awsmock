//
// Created by vogje01 on 12/17/24.
//

// AwsMock includes
#include <awsmock/service/frontend/FrontendServer.h>

namespace AwsMock::Service::Frontend {

    void FrontendServer::operator()() {

        if (!Core::Configuration::instance().GetValue<bool>("awsmock.frontend.active")) {
            log_info << "Frontend server inactive";
            return;
        }

        try {
            _running = true;

            auto const address = net::ip::make_address(Core::Configuration::instance().GetValue<std::string>("awsmock.frontend.address"));
            unsigned short port = Core::Configuration::instance().GetValue<int>("awsmock.frontend.port");
            std::string doc_root = Core::Configuration::instance().GetValue<std::string>("awsmock.frontend.doc-root");
            const int num_workers = Core::Configuration::instance().GetValue<int>("awsmock.frontend.workers");

            boost::asio::io_context ioc{1};
            boost::asio::ip::tcp::acceptor acceptor{ioc, {address, port}};

            std::list<FrontendWorker> workers;
            for (int i = 0; i < num_workers; ++i) {
                workers.emplace_back(acceptor, doc_root);
                workers.back().Start();
            }

            log_info << "Frontend server started, endpoint: " << address << ":" << port << " workers: " << num_workers;
            while (_running) {
                ioc.poll();
            }

        } catch (const std::exception &e) {
            log_error << "Error: " << e.what() << std::endl;
        }
    }

}// namespace AwsMock::Service::Frontend