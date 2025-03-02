//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_HANDLER_H
#define AWSMOCK_SERVICE_LAMBDA_HANDLER_H

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/dto/common/LambdaClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/lambda/LambdaService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief Lambda request handler
     *
     * All lambda related REST call are ending here.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit LambdaHandler() : AbstractHandler("lambda-handler") {}

        /**
         * @brief HTTP GET request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::HandleGetRequest
         */
        http::response<http::dynamic_body> HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::HandleGetRequest
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief HTTP DELETE request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::HandleGetRequest
         */
        http::response<http::dynamic_body> HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * Lambda module
         */
        Service::LambdaService _lambdaService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_HANDLER_H
