//
// Created by vogje01 on 5/28/24.
//

#ifndef AWSMOCK_CORE_DOMAIN_SOCKET_RESULT_H
#define AWSMOCK_CORE_DOMAIN_SOCKET_RESULT_H

// C++ includes
#include <string>

// Boost includes
#include <boost/beast.hpp>

namespace AwsMock::Core {

    using namespace boost::beast::http;

    struct DomainSocketResult {

        /**
         * Status code
         */
        status statusCode;

        /**
         * Body
         */
        std::string body;

        /**
         * Content length
         */
        long contentLength;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_DOMAIN_SOCKET_RESULT_H
