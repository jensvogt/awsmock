//
// Created by vogje01 on 12/17/24.
//

// AwsMock includes
#include <awsmock/service/frontend/FrontendSession.h>

namespace AwsMock::Service::Frontend {

    FrontendSession::FrontendSession(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

    void FrontendSession::run() {
        dispatch(socket_.get_executor(), boost::beast::bind_front_handler(&FrontendSession::do_read, shared_from_this()));
    }

    void FrontendSession::do_read() {
        req_ = {};
        async_read(socket_, buffer_, req_, boost::beast::bind_front_handler(&FrontendSession::on_read, shared_from_this()));
    }

    void FrontendSession::on_read(const boost::beast::error_code &ec, std::size_t) {
        if (ec == boost::beast::http::error::end_of_stream)
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
    void FrontendSession::handle_request() {
        namespace fs = std::filesystem;

        auto doc_root = Core::Configuration::instance().GetValue<std::string>("awsmock.frontend.doc-root");
        fs::path root = fs::current_path() / doc_root;

        fs::path path;
        if (req_.target() == "/") {
            path = root / "index.html";
        } else {
            path = root / fs::path(std::string(req_.target())).relative_path();
        }

        // Angular SPA fallback: all 404s -> index.html
        if (bool exists = fs::exists(path) && !fs::is_directory(path); !exists) {
            path = root / "index.html";
        }

        std::string body = read_file(path);

        if (body.empty()) {
            boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, req_.version()};
            res.set(boost::beast::http::field::server, "Beast");
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "File not found";
            res.prepare_payload();
            return write_response(std::move(res));
        }

        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req_.version()};
        res.set(boost::beast::http::field::server, "Beast");
        res.set(boost::beast::http::field::content_type, mime_type(path.string()));
        res.body() = std::move(body);
        res.prepare_payload();

        write_response(std::move(res));
    }

    void FrontendSession::do_close() {
        boost::beast::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    }

}// namespace AwsMock::Service::Frontend