//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_SERVICE_GATEWAY_LISTENER_H
#define AWSMOCK_SERVICE_GATEWAY_LISTENER_H

// C++ includes
#include <functional>
#include <memory>

// Boost includes
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
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
         */
        GatewayListener() = default;

        /**
         * @brief The new connection gets its own strand
         *
         * @param ioc Boost IO context
         * @param endpoint HTTP endpoint
         * @param yield yield the current thread
         */
        void DoListen(boost::asio::io_context &ioc, ip::tcp::endpoint &endpoint, boost::asio::yield_context yield);
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_GATEWAY_LISTENER_H
