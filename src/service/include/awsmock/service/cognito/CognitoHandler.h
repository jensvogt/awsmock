//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_COGNITO_HANDLER_H
#define AWSMOCK_SERVICE_COGNITO_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/dto/common/CognitoClientCommand.h>
#include <awsmock/service/cognito/CognitoService.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Cognito HTTP handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CognitoHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        CognitoHandler() : AbstractHandler("cognito-handler") {}

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

      private:

        /**
         * @brief Return the command from the header.
         *
         * @param request HTTP request
         * @return Cognito action
         */
        static Dto::Common::CognitoCommandType GetActionFromHeader(const http::request<http::dynamic_body> &request);

        /**
         * Cognito service
         */
        CognitoService _cognitoService;

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

#endif// AWSMOCK_SERVICE_COGNITO_HANDLER_H
