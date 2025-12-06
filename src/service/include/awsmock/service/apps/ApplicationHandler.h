//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_APPLICATION_HANDLER_H
#define AWSMOCK_SERVICE_APPLICATION_HANDLER_H

// AwsMock includes
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/dto/common/ApplicationClientCommand.h>
#include <awsmock/service/apps/ApplicationService.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>


namespace AwsMock::Service {

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
        explicit ApplicationHandler(boost::asio::io_context &ioc) : AbstractHandler("application-handler", ioc), _applicationService(ioc) {}

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
         * Application service
         */
        ApplicationService _applicationService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_APPLICATION_HANDLER_H
