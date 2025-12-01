//
// Created by vogje01 on 12/17/24.
//

// AwsMock includes
#include <awsmock/service/frontend/FrontendServer.h>
#include <boost/asio/signal_set.hpp>

#ifdef _WIN32
extern HANDLE g_ServiceStopEvent;
#endif

namespace AwsMock::Service::Frontend {
    //--------------------------------------------------------------------
    // MIME detection
    //--------------------------------------------------------------------
    std::string mime_type(std::string const &path) {
        using beast::iequals;
        auto ext = std::filesystem::path(path).extension().string();
        if (iequals(ext, ".htm") || iequals(ext, ".html")) return "text/html";
        if (iequals(ext, ".css")) return "text/css";
        if (iequals(ext, ".js")) return "application/javascript";
        if (iequals(ext, ".json")) return "application/json";
        if (iequals(ext, ".png")) return "image/png";
        if (iequals(ext, ".jpg") || iequals(ext, ".jpeg")) return "image/jpeg";
        if (iequals(ext, ".svg")) return "image/svg+xml";
        if (iequals(ext, ".ico")) return "image/x-icon";
        return "application/octet-stream";
    }

    //--------------------------------------------------------------------
    // Read file into string
    //--------------------------------------------------------------------
    std::string read_file(const std::filesystem::path &p) {
        std::ifstream f(p, std::ios::binary);
        if (!f) return {};
        return {std::istreambuf_iterator(f), {}};
    }

    //--------------------------------------------------------------------
    // HTTP session: reads req → serves Angular files → sends response
    //--------------------------------------------------------------------
    class session : public std::enable_shared_from_this<session> {
        tcp::socket socket_;
        beast::flat_buffer buffer_;
        http::request<http::string_body> req_;

      public:

        explicit session(tcp::socket socket)
            : socket_(std::move(socket)) {}

        void run() {
            net::dispatch(socket_.get_executor(),
                          beast::bind_front_handler(&session::do_read, shared_from_this()));
        }

      private:

        void do_read() {
            req_ = {};
            http::async_read(socket_, buffer_, req_,
                             beast::bind_front_handler(&session::on_read,
                                                       shared_from_this()));
        }

        void on_read(beast::error_code ec, std::size_t) {
            if (ec == http::error::end_of_stream)
                return do_close();
            if (ec) {
                std::cerr << "read: " << ec.message() << "\n";
                return;
            }
            handle_request();
        }

        //----------------------------------------------------------------
        // SERVE ANGULAR SPA
        //----------------------------------------------------------------
        void handle_request() {
            namespace fs = std::filesystem;

            auto doc_root = Core::Configuration::instance().GetValue<std::string>("awsmock.frontend.doc-root");
            fs::path root = fs::current_path() / doc_root;

            fs::path path;
            if (req_.target() == "/") {
                path = root / "index.html";
            } else {
                path = root / fs::path(std::string(req_.target())).relative_path();
            }

            bool exists = fs::exists(path) && !fs::is_directory(path);

            // Angular SPA fallback: all 404s -> index.html
            if (!exists) {
                path = root / "index.html";
            }

            std::string body = read_file(path);

            if (body.empty()) {
                http::response<http::string_body> res{
                        http::status::not_found, req_.version()};
                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "File not found";
                res.prepare_payload();
                return write_response(std::move(res));
            }

            http::response<http::string_body> res{
                    http::status::ok, req_.version()};
            res.set(http::field::server, "Beast");
            res.set(http::field::content_type, mime_type(path));
            res.body() = std::move(body);
            res.prepare_payload();

            write_response(std::move(res));
        }

        //----------------------------------------------------------------
        // Write HTTP response
        //----------------------------------------------------------------
        template<class Body>
        void write_response(http::response<Body> &&res) {
            auto sp = std::make_shared<http::response<Body>>(std::move(res));
            http::async_write(socket_, *sp,
                              [self = shared_from_this(), sp](beast::error_code ec, std::size_t) {
                                  if (ec) {
                                      std::cerr << "write: " << ec.message() << "\n";
                                  }
                                  self->do_close();
                              });
        }

        void do_close() {
            beast::error_code ec;
            socket_.shutdown(tcp::socket::shutdown_send, ec);
        }
    };

    //--------------------------------------------------------------------
    // Listener: accepts new connections, spawns sessions
    //--------------------------------------------------------------------
    class listener : public std::enable_shared_from_this<listener> {
        net::io_context &ioc_;
        tcp::acceptor acceptor_;

      public:

        listener(net::io_context &ioc, tcp::endpoint endpoint)
            : ioc_(ioc), acceptor_(net::make_strand(ioc)) {
            beast::error_code ec;

            acceptor_.open(endpoint.protocol(), ec);
            acceptor_.set_option(net::socket_base::reuse_address(true), ec);
            acceptor_.bind(endpoint, ec);
            acceptor_.listen(net::socket_base::max_listen_connections, ec);

            if (ec) throw beast::system_error(ec);
        }

        void run() {
            do_accept();
        }

      private:

        void do_accept() {
            acceptor_.async_accept(
                    net::make_strand(ioc_),
                    beast::bind_front_handler(&listener::on_accept,
                                              shared_from_this()));
        }

        void on_accept(beast::error_code ec, tcp::socket socket) {
            if (!ec)
                std::make_shared<session>(std::move(socket))->run();
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

            auto const address = net::ip::make_address(Core::Configuration::instance().GetValue<std::string>("awsmock.frontend.address"));
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

            tcp::endpoint endpoint{tcp::v4(), port};

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

}// namespace AwsMock::Service::Frontend