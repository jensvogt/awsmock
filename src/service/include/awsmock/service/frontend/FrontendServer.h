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
#include <awsmock/core/Macros.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/service/frontend/FrontendWorker.h>

namespace AwsMock::Service::Frontend {

    class FrontendServer {

      public:

        /**
         * Constructor
         */
        AWSMOCK_API FrontendServer() = default;

        /**
         * @brief HTTP request worker
         */
        AWSMOCK_API void operator()() const;
    };

}// namespace AwsMock::Service::Frontend

#endif// AWSMOCK_SERVICE_FRONTEND_HTTP_SERVER_H
