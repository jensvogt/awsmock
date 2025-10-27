//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVER_GATEWAY_SERVER_H
#define AWSMOCK_SERVER_GATEWAY_SERVER_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/asio/io_context.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/gateway/GatewayListener.h>

namespace beast = boost::beast;  // from <boost/beast.hpp>
namespace http = beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;     // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;// from <boost/asio/ip/tcp.hpp>

namespace AwsMock::Service {

    namespace ip = boost::asio::ip;

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
         * @brief Gracefully shutdown the server
         */
        void Shutdown() override;

      private:

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
         * Boost IO service
         */
        boost::asio::io_context &_ios;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_GATEWAY_SERVER_H
