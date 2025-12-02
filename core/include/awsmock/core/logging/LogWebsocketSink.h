//
// Created by vogje01 on 7/24/25.
//

#ifndef AWS_MOCK_CORE_LOG_WEBSOCKET_SINK_H
#define AWS_MOCK_CORE_LOG_WEBSOCKET_SINK_H

// C++ includes
#include <fstream>
#include <iosfwd>

// Boost includes
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/log/attributes/value_visitation.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

// AwsMock includes

namespace AwsMock::Core {

    class LogWebsocketSink : public boost::log::sinks::basic_formatted_sink_backend<char> {

      public:

        /**
         * @brief The constructor initializes the internal data
         *
         * @param ws websocket
         */
        explicit LogWebsocketSink(const boost::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> &ws);

        /**
         * @brief The function consumes the log records that come from the frontend
         *
         * @param rec log record
         * @param message web socket message
         */
        void consume(boost::log::record_view const &rec, string_type const &message);

        /**
         * The function flushes the file
         */
        static void flush();

      private:

        /**
         * Web socket
         */
        boost::shared_ptr<boost::beast::websocket::stream<boost::beast::basic_stream<boost::asio::ip::tcp>>> _ws;

        /**
         * Read buffer
         */
        boost::beast::flat_buffer _buffer;
    };

    typedef boost::log::sinks::synchronous_sink<LogWebsocketSink> webSocketSink_t;

}// namespace AwsMock::Core

#endif//AWS_MOCK_CORE_LOG_WEBSOCKET_SINK_H
