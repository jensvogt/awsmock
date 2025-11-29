//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_CONTAINER_HANDLER_H
#define AWSMOCK_SERVICE_CONTAINER_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/common/CognitoClientCommand.h>
#include <awsmock/service/cognito/CognitoService.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Container HTTP handler
     *
     * @pre
     * This handler proxies the connection from outside to the local docker daemon.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ContainerHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit ContainerHandler(boost::asio::io_context &ioc) : AbstractHandler("container-handler", ioc) {}

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

      private:

        /**
         * @brief Container service
         */
        ContainerService _containerService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_CONTAINER_HANDLER_H
