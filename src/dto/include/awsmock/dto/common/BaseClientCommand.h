//
// Created by vogje01 on 5/12/24.
//

#ifndef AWSMOCK_DTO_BASE_CLIENT_COMMAND_H
#define AWSMOCK_DTO_BASE_CLIENT_COMMAND_H

// Boost includes
#include <boost/beast/http/verb.hpp>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;

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

}// namespace AwsMock::Dto::Common

#endif//AWSMOCK_DTO_BASE_CLIENT_COMMAND_H
