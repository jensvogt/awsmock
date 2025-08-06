//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVER_GATEWAY_SERVER_H
#define AWSMOCK_SERVER_GATEWAY_SERVER_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/gateway/GatewayListener.h>

namespace AwsMock::Service {

    namespace beast = boost::beast;// from <boost/beast.hpp>
    namespace http = beast::http;  // from <boost/beast/http.hpp>
    namespace net = boost::asio;   // from <boost/asio.hpp>
    namespace ip = boost::asio::ip;
    using tcp = ip::tcp;// from <boost/asio/ip/tcp.hpp>
    using tcp_stream_t = beast::tcp_stream::rebind_executor<net::use_awaitable_t<>::executor_with_default<net::any_io_executor>>::other;

    /**
     * @brief Gateway server
     *
     * The Gateway server acting as an API gateway for the different AWS services. Per default, it runs on port 4566. To use a different port, set the port in the
     * ```awsmock.properties``` file. The gateway has an internal routing table for the different AWS modules. Each module runs ona different port starting with
     * 9500. Per default the server runs with 50 threads, which means 50 connections can be handled simultaneously. If you need more concurrent connection, set
     * the ```awsmock.service.gateway.http.max.threads``` in the properties file.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class GatewayServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit GatewayServer(boost::asio::io_context &ios);

        /**
         * @brief Socket listener
         *
         * @param endpoint HTTP endpoint
         * @return awaitable
         */
        boost::asio::awaitable<void> DoListen(ip::tcp::endpoint endpoint);

        /**
         * @brief Session handling
         *
         * @param stream input stream
         * @return awaitable
         */
        boost::asio::awaitable<void> DoSession(tcp_stream_t stream);

      private:

        /**
         * @brief Return a response for the given request.
         *
         * The concrete type of the response message (which depends on the request), is type-erased in message_generator.
         *
         * @tparam Body HTTP body
         * @tparam Allocator allocator
         * @param stream response stream
         * @param request HTTP request
         * @param alreadyResponded true if the handler responded already
         * @return
         */
        template<class Body, class Allocator>
        http::message_generator HandleRequest(tcp_stream_t stream, http::request<Body, http::basic_fields<Allocator>> &&request, bool &alreadyResponded);

        /**
         * @brief Returns the authorization header
         *
         * @param request HTTP request
         * @param secretAccessKey AWS secret access key
         * @return AuthorizationHeaderKeys
         * @see AuthorizationHeaderKeys
         */
        static Core::AuthorizationHeaderKeys GetAuthorizationKeys(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey);

        /**
         * @brief Handles options request
         *
         * @param request HTTP request
         * @return options response
         */
        static http::response<http::dynamic_body> HandleOptionsRequest(const http::request<http::dynamic_body> &request);

        /**
         * @brief Handles continue request (HTTP status: 100)
         *
         * @param _stream HTTP socket stream
         */
        static void HandleContinueRequest(beast::tcp_stream &_stream);
        /**
         * Verify signature flag
         */
        bool _verifySignature;

        /**
         * Default region
         */
        std::string _region;

        /**
         * Default user
         */
        std::string _user;

        /**
         * Rest port
         */
        unsigned short _port;

        /**
         * Rest host
         */
        std::string _host;

        /**
         * HTTP address
         */
        std::string _address;

        /**
         * Request timeout
         */
        int _timeout = 900;

        /**
         * Boost IO service
         */
        boost::asio::io_context &_ios;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_GATEWAY_SERVER_H
