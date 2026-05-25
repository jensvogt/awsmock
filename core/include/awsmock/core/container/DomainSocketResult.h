//
// Created by vogje01 on 5/28/24.
//

#pragma once

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
