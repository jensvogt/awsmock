//
// Created by vogje01 on 12/17/24.
//

// AwsMock includes
#include "awsmock/service/frontend/FrontendSession.h"


#include <awsmock/service/frontend/FrontendServer.h>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>

#ifdef _WIN32
extern HANDLE g_ServiceStopEvent;
#endif

namespace AwsMock::Service::Frontend {

    //--------------------------------------------------------------------
    // Listener: accepts new connections, spawns sessions
    //--------------------------------------------------------------------
    class listener : public std::enable_shared_from_this<listener> {
        net::io_context &_ioc;
        tcp::acceptor _acceptor;

    public:
        listener(net::io_context &ioc, const tcp::endpoint &endpoint)
            : _ioc(ioc), _acceptor(boost::asio::make_strand(ioc)) {
            beast::error_code ec;

            ec = _acceptor.open(endpoint.protocol(), ec);
            if (ec) {
                log_error << ec.message();
                return;
            }

            ec = _acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
            if (ec) {
                log_error << ec.message();
                return;
            }

            // Dual-stack: also accept IPv4-mapped connections on the IPv6 socket
            ec = _acceptor.set_option(boost::asio::ip::v6_only(false), ec);
            if (ec) {
                log_error << ec.message();
                return;
            }

            ec = _acceptor.bind(endpoint, ec);
            if (ec) {
                log_error << ec.message();
                return;
            }

            ec = _acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
            if (ec) {
                log_error << ec.message();
                return;
            }
        }

        void run() {
            do_accept();
        }

    private:
        void do_accept() {
            _acceptor.async_accept(
                boost::asio::make_strand(_ioc),
                beast::bind_front_handler(&listener::on_accept,
                                          shared_from_this()));
        }

        void on_accept(const beast::error_code &ec, tcp::socket socket) {
            if (!ec)
                std::make_shared<FrontendSession>(std::move(socket))->run();
            do_accept();
        }
    };

    void FrontendServer::operator()(bool isService) {

        if (!Core::Configuration::instance().GetValue<bool>("awsmock.frontend.active")) {
            log_info << "Frontend server inactive";
            return;
        }

        try {
            _running = true;

            auto const address = Core::Configuration::instance().GetValue<std::string>("awsmock.frontend.address");
            const unsigned short port = Core::Configuration::instance().GetValue<int>("awsmock.frontend.port");
            auto doc_root = Core::Configuration::instance().GetValue<std::string>("awsmock.frontend.doc-root");
            const int num_workers = Core::Configuration::instance().GetValue<int>("awsmock.frontend.workers");

            net::io_context ioc{num_workers};

            // Capture SIGINT and SIGTERM to perform a clean shutdown
            boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&](beast::error_code const &, int) {
                // Stop the `io_context`. This will cause `run()` to return immediately,
                // eventually destroying the `io_context` and all the sockets in it.
                log_info << "Frontend stopping on signal";
                ioc.stop();
                log_info << "Frontend IO context stopped";
            });
            log_info << "Frontend signal handler installed";

            auto endpoint = tcp::endpoint(boost::asio::ip::make_address(address), port);
            std::make_shared<listener>(ioc, endpoint)->run();

            // thread pool
            std::vector<std::thread> pool;
            pool.reserve(num_workers);

            for (int i = 0; i < num_workers; ++i) {
                pool.emplace_back([&ioc] {
                    ioc.run();
                });
            }

#ifdef _WIN32
            if (isService) {
                while (true) {
                    ioc.run_for(std::chrono::seconds(1));
                    if (WaitForSingleObject(g_ServiceStopEvent, 0) == WAIT_OBJECT_0) {
                        break;
                    }
                }

                // Stop io context
                ioc.stop();
                log_info << "Frontend server stopped";

            } else {
                ioc.run();
            }

#else

            // Wait for all threads
            for (auto &t: pool) t.join();

            log_info << "Frontend server started, endpoint: " << address << ":" << port << " workers: " << num_workers;
            ioc.run();
#endif
        } catch (const std::exception &e) {
            log_error << "Error: " << e.what() << std::endl;
        }
    }

} // namespace AwsMock::Service::Frontend
