//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_S3_HANDLER_H
#define AWSMOCK_SERVICE_S3_HANDLER_H

// C*+ includes
#include <chrono>

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/S3ClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief S3 request handler
     *
     * AWS S3 HTTP request handler. All S3 related REST call are ending here. Depending on the request header the S3 module will be selected in case the
     * authorization header contains the S3 module. As the different clients (Java, C++, Python, nodejs20) are using different request structure, the request
     * are first send to the S3CmdHandler, which normalizes the commands.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3Handler : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit S3Handler() : AbstractHandler("s3-handler"){};

        /**
         * @brief HTTP GET request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handleGet(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP PUT request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePut(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandlePutRequest(http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief Delete DELETE request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handleDelete(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief Head request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return response HTTP response
         * @see AbstractResource::handleHead(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandleHeadRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * @brief Get the range for a range request
         *
         * @param request HTTP request
         * @param min minimum byte
         * @param max maximum byte
         * @param size content length
         */
        static void GetRange(const http::request<http::dynamic_body> &request, long &min, long &max, long &size);

        /**
         * @brief Returns the metadata has string key/value map.
         *
         * @par
         * Only headers with keys starting with x-amz-meta are included in the map.
         *
         * @param request HTTP request
         * @return hash map of key/value pairs.
         */
        static std::map<std::string, std::string> GetMetadata(const http::request<http::dynamic_body> &request);

        /**
         * @brief Prepare the body for further processing
         *
         * @par AWS puts some extra bytes into the response when chunk encoding is used.
         *
         * @param request HTTP request
         * @param sb prepared strean buffer
         */
        static long PrepareBody(http::request<http::dynamic_body> &request, boost::beast::net::streambuf &sb);

        /**
         * S3 service
         */
        S3Service _s3Service;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_S3_HANDLER_H
