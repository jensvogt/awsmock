//
// Created by vogje01 on 5/12/24.
//

#pragma once
// C++ standard includes
#include <map>

// Boost includes
#include <boost/beast/http/verb.hpp>

namespace Awsmock::Dto::Common {

    namespace http = boost::beast::http;

    /**
     * @brief Base client command
     *
     * @par
     * Defines some common attributes, like url, HTTP method, headers, payload, contentLength.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class BaseClientCommand {

      public:

        /**
         * HTTP URL
         */
        std::string url;

        /**
         * HTTP request type
         */
        http::verb method;

        /**
         * HTTP headers
         */
        std::map<std::string, std::string> headers;

        /**
         * HTTP message body
         */
        std::string payload;

        /**
         * HTTP content type
         */
        std::string contentType;

        /**
         * HTTP content length
         */
        long contentLength;

        /**
         * Client region
         */
        std::string region;

        /**
         * Client user
         */
        std::string user;

        /**
         * Request ID
         */
        std::string requestId;
    };

}// namespace Awsmock::Dto::Common

