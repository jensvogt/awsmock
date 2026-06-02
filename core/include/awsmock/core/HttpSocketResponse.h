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
    };

}// namespace Awsmock::Core
