//
// Created by vogje01 on 5/28/24.
//

#pragma once

// C++ includes
#include <string>

// Boost includes
#include <boost/beast.hpp>

namespace Awsmock::Core {

    struct HttpSocketResponse {

        /**
         * Status code
         */
        boost::beast::http::status statusCode;

        /**
         * Body
         */
        std::string body;

        /**
         * Headers
         */
        std::map<std::string, std::string> headers;

        /**
         * Set when the request failed at the socket/transport level (connect, write, or read
         * error) rather than receiving a valid HTTP response. Callers can use this to
         * distinguish "peer unreachable/unresponsive" from a legitimate application-level
         * error status.
         */
        bool networkError = false;
    };

}// namespace Awsmock::Core
