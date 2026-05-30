//
// Created by vogje01 on 12/17/24.
//

#pragma once

// C++ standard includes
#include <list>
#include <string>

// Boost includes
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/v6_only.hpp>

// AwsMock includes
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/frontend/FrontendSession.h>
#include <awsmock/service/frontend/FrontendWorker.h>

namespace AwsMock::Service::Frontend {

    class FrontendServer {

    public:
        /**
         * Constructor
         */
        FrontendServer() = default;

        /**
         * @brief Stop signal handler
         */
        void Stop() { _running = false; }

        /**
         * @brief HTTP request worker
         */
        void operator()(bool isService);

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Frontend"};

        /**
         * Running flag
         */
        bool _running = false;
    };

} // namespace AwsMock::Service::Frontend
