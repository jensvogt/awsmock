//
// Created by vogje01 on 7/19/25.
//

#include <awsmock/service/application/ApplicationLogSession.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Application"};
}

namespace Awsmock::Service {
    namespace http = boost::beast::http;

    void ApplicationLogSession::Run() {

        _ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
        _ws.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type &res) {
            res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
        }));

        // Read the HTTP upgrade request before promoting to WebSocket so we can
        // inspect the URL query string for the containerId parameter.
        http::request<http::string_body> req;
        boost::beast::error_code ec;
        http::read(_ws.next_layer(), _buffer, req, ec);
        if (ec) {
            log_error << "HTTP upgrade read failed: " << ec.message();
            return;
        }

        // Parse containerId from the URL query string (e.g. "/?containerId=abc123")
        const std::string target{req.target()};
        std::string containerId;
        if (const auto qpos = target.find('?'); qpos != std::string::npos) {
            const std::string query = target.substr(qpos + 1);
            if (const auto cidpos = query.find("containerId="); cidpos != std::string::npos) {
                containerId = query.substr(cidpos + 12);// len("containerId=") == 12
                if (const auto amppos = containerId.find('&'); amppos != std::string::npos) {
                    containerId = containerId.substr(0, amppos);
                }
            }
        }

        if (containerId.empty()) {
            log_error << "Missing containerId query parameter in WebSocket URL";
            return;
        }

        // Complete the WebSocket upgrade handshake
        _ws.accept(req, ec);
        if (ec) {
            log_error << "WebSocket accept failed: " << ec.message();
            return;
        }

        log_info << "Application log session started, containerId: " << containerId;
        ContainerService::instance().ContainerAttach(containerId, _ws, 1000);
    }

}// namespace Awsmock::Service
