//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_SERVICE_GATEWAY_LISTENER_H
#define AWSMOCK_SERVICE_GATEWAY_LISTENER_H

// C++ includes
#include <memory>

// Boost includes
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/gateway/GatewaySession.h>

namespace AwsMock::Service {

    /**
     * @brief Accepts incoming connections and launches the sessions
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class GatewayListener : public std::enable_shared_from_this<GatewayListener> {

      public:

        /**
         * @brief Constructor
         *
         * @param ioc Boost IO context
         * @param endpoint HTTP endpoint
         */
        GatewayListener(boost::asio::io_context &ioc, const ip::tcp::endpoint &endpoint);

        /**
         * @brief Start accepting incoming connections,
         *
         * We need to be executing within a strand to perform async operations on the I/O objects in this session. Although not strictly necessary
         * for single-threaded contexts, this example code is written to be thread-safe by default.
         */
        void Run();

      private:

        /**
         * @brief The new connection gets its own strand
         */
        void DoAccept();

        /**
         * @brief Accept callback
         *
         * @param ec error code
         * @param socket HTTP socket
         */
        void OnAccept(const boost::beast::error_code &ec, ip::tcp::socket socket);

        /**
         * Boost IO context
         */
        boost::asio::io_context &_ioc;

        /**
         * Boost acceptor
         */
        ip::tcp::acceptor _acceptor;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_GATEWAY_LISTENER_H
