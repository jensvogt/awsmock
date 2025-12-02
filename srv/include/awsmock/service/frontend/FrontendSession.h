//
// Created by vogje01 on 12/17/24.
//

// AwsMock includes
#ifndef AWSMOCK_SERVICE_FRONTEND_HTTP_SESSION_H
#define AWSMOCK_SERVICE_FRONTEND_HTTP_SESSION_H

// C*+ includes
#include <filesystem>
#include <fstream>
#include <iostream>

// Boost includes
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body_fwd.hpp>

// AwsMock include
#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Service::Frontend {

    //--------------------------------------------------------------------
    // MIME detection
    //--------------------------------------------------------------------
    inline std::string mime_type(std::string const &path) {
        using boost::beast::iequals;
        const auto ext = std::filesystem::path(path).extension().string();
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
    inline std::string read_file(const std::filesystem::path &p) {
        std::ifstream f(p, std::ios::binary);
        if (!f) return {};
        return {std::istreambuf_iterator(f), {}};
    }

    //--------------------------------------------------------------------
    // HTTP session: reads req → serves Angular files → sends response
    //--------------------------------------------------------------------
    class FrontendSession : public std::enable_shared_from_this<FrontendSession> {

        boost::asio::ip::tcp::socket socket_;
        boost::beast::flat_buffer buffer_;
        boost::beast::http::request<boost::beast::http::string_body> req_;

      public:

        explicit FrontendSession(boost::asio::ip::tcp::socket socket);

        void run();

      private:

        void do_read();

        void on_read(const boost::beast::error_code &ec, std::size_t);

        //----------------------------------------------------------------
        // SERVE ANGULAR SPA
        //----------------------------------------------------------------
        void handle_request();

        //----------------------------------------------------------------
        // Write HTTP response
        //----------------------------------------------------------------
        template<class Body>
        void write_response(boost::beast::http::response<Body> &&res) {
            auto sp = std::make_shared<boost::beast::http::response<Body>>(std::move(res));
            boost::beast::http::async_write(socket_, *sp, [self = shared_from_this(), sp](const boost::beast::error_code &ec, std::size_t) {
                if (ec) {
                    log_error << "Error writing: " << ec.message();
                }
                self->do_close();
            });
        }

        void do_close();
    };

}// namespace AwsMock::Service::Frontend

#endif// AWSMOCK_SERVICE_FRONTEND_HTTP_SESSION_H