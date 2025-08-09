//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_HANDLER_H
#define AWSMOCK_SERVICE_APPLICATION_HANDLER_H

// Boost includes
#include <boost/asio/detached.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/common/ApplicationClientCommand.h>
#include <awsmock/dto/common/CognitoClientCommand.h>
#include <awsmock/service/apps/ApplicationService.h>
#include <awsmock/service/cognito/CognitoService.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Application HTTP handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit ApplicationHandler(boost::asio::io_context &ioc) : AbstractHandler("application-handler", ioc) {}

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractHandler::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * Application service
         */
        ApplicationService _applicationService;

        /**
         * Boost IO context
         */
        boost::asio::io_context _ioc;

        /**
         * @brief Create static default headers map.
         *
         * @return default header map
         */
        static std::map<std::string, std::string> CreateHeaderMap() {
            std::map<std::string, std::string> defaultHeaders;
            defaultHeaders[to_string(http::field::access_control_allow_origin)] = "http://localhost:4200";
            defaultHeaders[to_string(http::field::access_control_allow_headers)] = "cache-control,content-type,x-amz-target,x-amz-user-agent";
            defaultHeaders[to_string(http::field::access_control_allow_methods)] = "GET,PUT,POST,DELETE,HEAD,OPTIONS";
            return defaultHeaders;
        }
        static const std::map<std::string, std::string> headers;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_HANDLER_H
