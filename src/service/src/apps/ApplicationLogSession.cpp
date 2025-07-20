//
// Created by vogje01 on 7/19/25.
//

#include <awsmock/service/apps/ApplicationLogSession.h>

namespace AwsMock::Service {

    void ApplicationLogSession::Run() {

        // Set suggested timeout settings for the websocket
        ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type &res) {
            res.set(boost::beast::http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
        }));

        // Accept the websocket handshake
        ws_.async_accept(boost::beast::bind_front_handler(&ApplicationLogSession::OnAccept, shared_from_this()));
    }

    void ApplicationLogSession::OnAccept(const boost::beast::error_code &ec) {
        if (ec)
            log_error << "Accept failed, errorCode: " << ec.message() << std::endl;

        // Read a message
        DoRead();
    }

    void ApplicationLogSession::DoRead() {

        // Read a message into our buffer
        ws_.async_read(buffer_, boost::beast::bind_front_handler(&ApplicationLogSession::OnRead, shared_from_this()));
    }

    void ApplicationLogSession::OnRead(const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        // This indicates that the session was closed
        if (ec == boost::beast::websocket::error::closed) {
            log_info << "Application log session closed";
            return;
        }

        if (ec) {
            log_info << "Websocket closed by peer, errorCode: " << ec.message();
            return;
        }

        // Echo the message
        ws_.text(ws_.got_text());
        std::string response = HandleEvent(boost::beast::buffers_to_string(buffer_.cdata()), ws_);
        ws_.async_write(boost::asio::buffer(response), boost::beast::bind_front_handler(&ApplicationLogSession::OnWrite, shared_from_this()));
    }

    void ApplicationLogSession::OnWrite(const boost::beast::error_code &ec, std::size_t bytes_transferred) {

        boost::ignore_unused(bytes_transferred);

        if (ec) {
            log_info << "Websocket closed by peer, errorCode: " << ec.message();
            return;
        }

        // Clear the buffer
        log_info << "Write: " << boost::beast::make_printable(buffer_.data());
        buffer_.consume(buffer_.size());

        // Do another read
        DoRead();
    }

    std::string ApplicationLogSession::HandleEvent(const std::string &message, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws) {

        const Dto::Apps::WebSocketCommand webSocketCommand = Dto::Apps::WebSocketCommand::FromJson(message);
        log_debug << "Received message: " << webSocketCommand;
        switch (webSocketCommand.command) {
            case Dto::Apps::WebSoketCommandType::OPEN_LOG:
                ContainerService::instance().ContainerAttach(webSocketCommand.containerId, ws);
                return "ok";
            default:
                return "ok";
        }
    }
}// namespace AwsMock::Service