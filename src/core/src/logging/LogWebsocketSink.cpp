//
// Created by vogje01 on 7/24/25.
//

#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/logging/LogWebsocketSink.h>

namespace AwsMock::Core {

    LogWebsocketSink::LogWebsocketSink(const boost::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> &ws) : _ws(ws) {
        if (_ws) {
            _ws->control_callback([this](const boost::beast::websocket::frame_type kind, const boost::beast::string_view message) {
                if (kind == boost::beast::websocket::frame_type::close) {
                    LogStream::RemoveWebSocketSink();
                    _ws->close(message);
                    _ws.reset();
                } else if (kind == boost::beast::websocket::frame_type::ping) {
                    _ws->pong("alive");
                }
            });
        }
    }

    void LogWebsocketSink::consume(boost::log::record_view const &rec, string_type const &message) {

        try {
            if (_ws->is_open()) {
                _ws->text(true);
                _ws->write(boost::asio::buffer(message));
            }
        } catch (boost::system::system_error &ex) {
            _buffer.consume(_buffer.size());
            _ws->close(boost::string_view("Websocket closed by peer").data());
        } catch (std::exception &ex) {
            _buffer.consume(_buffer.size());
            _ws->close(boost::string_view("Websocket closed by peer").data());
        }
    }

    void LogWebsocketSink::flush() {
    }
}// namespace AwsMock::Core