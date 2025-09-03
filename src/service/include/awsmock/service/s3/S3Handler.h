//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_S3_HANDLER_H
#define AWSMOCK_SERVICE_S3_HANDLER_H

// C*+ includes
#include <chrono>

// Boost includes
#include <boost/asio/detached.hpp>
#include <boost/asio/spawn.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/dto/common/S3ClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>
#include <awsmock/dto/s3/internal/DeleteObjectsRequest.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief S3 request handler
     *
     * AWS S3 HTTP request handler. All S3 related REST call are ending here. Depending on the request header the S3 module will be selected in case the
     * authorization header contains the S3 module. As the different clients (Java, C++, Python, nodejs20) are using different request structures, the requests
     * are first sent to the S3CmdHandler, which normalizes the commands.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3Handler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit S3Handler(boost::asio::io_context &ioc) : AbstractHandler("s3-handler", ioc), _s3Service(ioc) {}

        /**
         * @brief HTTP GET request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handleGet(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP PUT request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePut(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandlePutRequest(http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief Delete DELETE request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handleDelete(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief Head request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return response HTTP response
         * @see AbstractResource::handleHead(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
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
         * @param sb prepared stream buffer
         */
        static long PrepareBody(http::request<http::dynamic_body> &request, boost::beast::net::streambuf &sb);

        /**
         * @brief Gets bucket and key from the source header
         *
         * @param path path from source header
         * @param bucket bucket name
         * @param key S3 object key
         */
        static void GetBucketKeyFromHeader(const std::string &path, std::string &bucket, std::string &key);

        /**
         * S3 service
         */
        S3Service _s3Service;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_S3_HANDLER_H
