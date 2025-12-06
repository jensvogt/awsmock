//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_SERVICE_MODULE_HANDLER_H
#define AWSMOCK_SERVICE_MODULE_HANDLER_H

// AwsMock includes
#include <awsmock/dto/module/ListModuleNamesResponse.h>
#include <awsmock/dto/module/model/GatewayConfig.h>
#include <awsmock/dto/transfer/ListUsersRequest.h>
#include <awsmock/dto/transfer/model/Server.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/transfer/TransferService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;

    /**
     * @brief HTTP handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ModuleHandler final : public AbstractHandler {

      public:

        /**
         * @brief Manager HTTP server
         */
        explicit ModuleHandler(boost::asio::io_context &ioc) : AbstractHandler("module-handler", ioc) {}

        /**
         * @brief Handler HTTP GET requests.
         *
         * Handles all GET requests.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return HTTP response structure
         */
        http::response<http::dynamic_body> HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

        /**
         * @brief Handler HTTP POST requests.
         *
         * Handles all POST requests.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return HTTP response structure
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * Module service
         */
        ModuleService _moduleService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_MODULE_HANDLER_H
