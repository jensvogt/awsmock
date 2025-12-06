//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_SSM_HANDLER_H
#define AWSMOCK_SERVICE_SSM_HANDLER_H

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/SSMClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/ssm/SSMService.h>

namespace AwsMock::Service {

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
        explicit SSMHandler(boost::asio::io_context &ioc) : AbstractHandler("ssm-handler", ioc) {}

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

        /**
         * SSM service
         */
        Service::SSMService _ssmService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SSM_HANDLER_H
