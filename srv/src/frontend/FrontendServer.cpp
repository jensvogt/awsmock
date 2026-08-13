//
// Created by vogje01 on 12/17/24.
//

// AwsMock includes

#include <awsmock/service/frontend/FrontendServer.h>

#ifdef _WIN32
extern HANDLE g_ServiceStopEvent;
#endif

namespace Awsmock::Service::Frontend {

    //--------------------------------------------------------------------
    // Listener: accepts new connections, spawns sessions
    //--------------------------------------------------------------------
    class listener : public std::enable_shared_from_this<listener> {
        net::io_context &_ioc;
        tcp::acceptor _acceptor;

      public:

        listener(net::io_context &ioc, const tcp::endpoint &endpoint) : _ioc(ioc), _acceptor(boost::asio::make_strand(ioc)) {
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

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Frontend"};

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

        if (!Core::Configuration::instance().get<bool>("awsmock.frontend.active")) {
            log_info << "Frontend server inactive";
            return;
        }

        try {
            _running = true;

            auto const address = Core::Configuration::instance().get<std::string>("awsmock.frontend.address");
            const unsigned short port = Core::Configuration::instance().get<int>("awsmock.frontend.port");
            auto doc_root = Core::Configuration::instance().get<std::string>("awsmock.frontend.doc-root");
            const int num_workers = Core::Configuration::instance().get<int>("awsmock.frontend.workers");

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

#ifdef _WIN32
            // Separate thread watches the service stop event and stops ioc when signaled.
            // Without this, ioc.run() below only returns via the SIGINT/SIGTERM handler
            // above, which is never delivered when running as a Windows service, so the
            // server (and the stack frame owning it) would outlive the caller and crash
            // the process on service stop.
            std::thread stopWatcher;
            if (isService) {
                stopWatcher = std::thread([&ioc, this] {
                    WaitForSingleObject(g_ServiceStopEvent, INFINITE);
                    log_info << "Frontend stop event received, stopping io_context.";
                    ioc.stop();
                });
            }
#endif

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

            log_info << "Frontend server started, endpoint: " << address << ":" << port << " workers: " << num_workers;
            ioc.run();

            for (auto &t: pool) {
                if (t.joinable()) {
                    t.join();
                }
            }

#ifdef _WIN32
            if (stopWatcher.joinable()) {
                SetEvent(g_ServiceStopEvent);
                stopWatcher.join();
            }
#endif
            log_info << "Frontend server stopped";
        } catch (const std::exception &e) {
            log_error << "Error: " << e.what() << std::endl;
        }
    }

}// namespace Awsmock::Service::Frontend
