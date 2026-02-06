//
// Created by vogje01 on 2/5/26.
//

#ifndef AWSMOCK_LOGGING_WEB_SOCKET_SESSION_MANAGER_H
#define AWSMOCK_LOGGING_WEB_SOCKET_SESSION_MANAGER_H

// C++ include
#include <mutex>
#include <set>

// Boost includes
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <utility>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace AwsMock::Service::Logging {


}// namespace AwsMock::Service::Logging

#endif//AWSMOCK_LOGGING_WEB_SOCKET_SESSION_MANAGER_H
