//
// Created by vogje01 on 04/01/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/service/apigateway/ApiGatewayService.h>
#include <awsmock/service/common/AbstractHandler.h>

namespace Awsmock::Service {

    /**
     * @brief API gateway HTTP handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit ApiGatewayHandler(boost::asio::io_context &ioc) : AbstractHandler("api-gateway-handler", ioc), _apiGatewayService(ioc) {}

        /**
         * @brief HTTP GET request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractHandler::HandleGet(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractHandler::handlePost(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP DELETE request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractHandler::HandleDelete(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

        /**
         * API gateway service
         */
        ApiGatewayService _apiGatewayService;
    };

}// namespace Awsmock::Service
