//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_ABSTRACT_HANDLER_H
#define AWSMOCK_SERVICE_ABSTRACT_HANDLER_H

// C++ includes
#include <string>
#include <utility>

// Boost includes
#include <boost/asio/buffer.hpp>
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/MemoryMappedFile.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/ForbiddenException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/S3ClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Abstract HTTP request handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AbstractHandler {

      public:

        virtual ~AbstractHandler() = default;

        /**
         * @brief Default User-defined Constructor
         *
         * @param name handler name
         * @param ioc boost asio IO context
         */
        explicit AbstractHandler(const std::string &name, boost::asio::io_context &ioc) : _ioc(ioc), _name(std::move(name)) {};

        /**
         * @brief Handles the HTTP method GET.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method PUT.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandlePutRequest(http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method POST.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method DELETE.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Handles the HTTP method HEAD.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user current user
         * @return HTTP response
         */
        virtual http::response<http::dynamic_body> HandleHeadRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Send a OK response (HTTP state code 200) for a multipart download/upload
         *
         * @param request HTTP request object
         * @param status HTTP status
         * @param fileName file to send
         * @param contentLength content length of the stream in bytes
         * @param headers HTTP header map values, added to the default headers
         * @return HTTP response
         */
        static http::response<http::dynamic_body> SendResponse(const http::request<http::dynamic_body> &request, const http::status &status, const std::string &fileName, long contentLength, const std::map<std::string, std::string> &headers = {});

        /**
         * @brief Send an OK response (HTTP state code 200) with an part of an output.
         *
         * @param request HTTP request
         * @param fileName file to send
         * @param min minimum position
         * @param max minimum position
         * @param size current part size
         * @param totalSize total size of the file
         * @param status HTTP status, partial_content for part of multi-part-upload, ok for range download
         * @param headers HTTP header map values, added to the default headers
         * @return HTTP response
         */
        static http::response<http::dynamic_body> SendResponse(const http::request<http::dynamic_body> &request, const std::string &fileName, long min, long max, long size, long totalSize, const http::status &status, const std::map<std::string, std::string> &headers = {});

        /**
         * @brief General response
         *
         * @param request HTTP request
         * @param status HTTP status
         * @param body message body
         * @param headers HTTP header map values, added to the default headers
         * @return HTTP response
         */
        static http::response<http::dynamic_body> SendResponse(const http::request<http::dynamic_body> &request, const http::status &status, const std::string &body = {}, const std::map<std::string, std::string> &headers = {});

        /**
         * Get the name
         */
        std::string name() { return _name; }

      protected:

        /**
         * Boost asio IO context
         */
        boost::asio::io_context &_ioc;

      private:

        /**
         * Handler name
         */
        std::string _name;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_ABSTRACT_HANDLER_H
