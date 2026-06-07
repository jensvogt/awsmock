//
// Created by vogje01 on 04/01/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/SSMClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/ssm/SSMService.h>

namespace Awsmock::Service {

    namespace http = http;
    namespace ip = boost::asio::ip;

    /**
     * @brief KMS request handler
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SSMHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit SSMHandler(boost::asio::io_context &ioc) : AbstractHandler("ssm-handler", ioc) {
        }

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "SSM"};

        /**
         * SSM service
         */
        Service::SSMService _ssmService;
    };

}// namespace Awsmock::Service
