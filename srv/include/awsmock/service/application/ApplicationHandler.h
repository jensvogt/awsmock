//
// Created by vogje01 on 04/01/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/CoreException.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/dto/common/ApplicationClientCommand.h>
#include <awsmock/service/application/ApplicationService.h>
#include <awsmock/service/common/AbstractHandler.h>

namespace Awsmock::Service {

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
        explicit ApplicationHandler(boost::asio::io_context &ioc) : AbstractHandler("application-handler", ioc), _applicationService() {
        }

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

        mutable logger_t _logger{boost::log::keywords::channel = "Application"};

        /**
         * Application service
         */
        ApplicationService _applicationService;
    };

}// namespace Awsmock::Service
