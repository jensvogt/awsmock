//
// Created by vogje01 on 07/04/2024.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_HANDLER_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/SecretsManagerClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/secretsmanager/SecretsManagerService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief AWS secrets manager mock handler.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerHandler final : public AbstractHandler {

      public:

        /**
         * Constructor
         */
        explicit SecretsManagerHandler(boost::asio::io_context &ioc) : AbstractHandler("secretsmanager-handler", ioc), _secretsManagerService(ioc) {}

        /**
         * HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return response HTTP response
         * @see AbstractResource::handlePost(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * Secrets manager module
         */
        SecretsManagerService _secretsManagerService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_HANDLER_H
