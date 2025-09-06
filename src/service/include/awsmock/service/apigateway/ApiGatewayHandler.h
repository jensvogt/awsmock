//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_API_GATEWAY_HANDLER_H
#define AWSMOCK_SERVICE_API_GATEWAY_HANDLER_H

// AwsMock includes
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/dto/apigateway/CreateRestApiRequest.h>
#include <awsmock/dto/apigateway/CreateRestApiResponse.h>
#include <awsmock/dto/common/ApiGatewayClientCommand.h>
#include <awsmock/service/apigateway/ApiGatewayService.h>
#include <awsmock/service/common/AbstractHandler.h>

namespace AwsMock::Service {

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

        /**
         * API gateway service
         */
        ApiGatewayService _apiGatewayService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_HANDLER_H
