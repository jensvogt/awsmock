//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_HANDLER_H
#define AWSMOCK_SERVICE_LAMBDA_HANDLER_H

// C++ includes
#include <memory>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/LambdaClientCommand.h>
#include <awsmock/dto/lambda/internal/DisableAllLambdasRequest.h>
#include <awsmock/dto/lambda/internal/DisableLambdaRequest.h>
#include <awsmock/dto/lambda/model/InvocationType.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/lambda/LambdaService.h>
#include <awsmock/service/module/ModuleService.h>

namespace AwsMock::Service {

    /**
     * @brief Lambda request handler
     *
     * @par
     * All lambda-related REST call are ending here.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit LambdaHandler(boost::asio::io_context &ioc) : AbstractHandler("lambda-handler", ioc) {
            _moduleService = std::make_shared<ModuleService>();
            _lambdaService.sigLambdaCodeUpdated.connect(
                    boost::signals2::signal<void(std::string)>::slot_type(&ModuleService::UpdateLambda, _moduleService.get(), std::placeholders::_1).track_foreign(_moduleService)// This is the 'magic' that prevents crashes!
            );
        }

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
         * @brief HTTP PUT request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::HandleGetRequest
         */
        http::response<http::dynamic_body> HandlePutRequest(http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

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
         * @brief Lambda module
         */
        LambdaService _lambdaService;

        /**
         * @brief module service
         */
        std::shared_ptr<ModuleService> _moduleService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_HANDLER_H
