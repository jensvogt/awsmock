//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_LOG_SERVER_H
#define AWSMOCK_SERVICE_APPLICATION_LOG_SERVER_H

// C++ includes
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// Boost includes
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

// AwsMock includes
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/service/apps/ApplicationLogListener.h>
#include <awsmock/service/common/AbstractServer.h>

namespace AwsMock::Service {

    /**
     * @brief Application log server
     *
     * @par
     * Provides a websocket server, from which the frontend can read application log messages
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationLogServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit ApplicationLogServer();

        /**
         * @brief Create a log server
         *
         * @param listenAddress list IP address
         * @param port websocket port
         */
        void operator()(const std::string &listenAddress, long port) const;

        /**
         * @brief Shutdown server
         */
        void Shutdown() override;

      private:

        /**
         * The io_context is required for all I/O
         */
        //boost::asio::io_context _ioc{5};
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_LOG_SERVER_H
