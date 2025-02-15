//
// Created by vogje01 on 12/17/24.
//

#ifndef AWSMOCK_SERVICE_FRONTEND_HTTP_SERVER_H
#define AWSMOCK_SERVICE_FRONTEND_HTTP_SERVER_H

// C++ standard includes
#include <list>
#include <string>

// Boost includes
#include <boost/asio/ip/tcp.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/service/frontend/FrontendWorker.h>

namespace AwsMock::Service::Frontend {

    class FrontendServer {

      public:

        /**
         * Constructor
         */
        FrontendServer() = default;

        /**
         * @brief HTTP request worker
         */
        void operator()() const;
    };

}// namespace AwsMock::Service::Frontend

#endif// AWSMOCK_SERVICE_FRONTEND_HTTP_SERVER_H
