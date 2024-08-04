//
// Created by vogje01 on 07/04/2024.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_HANDLER_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_HANDLER_H

// Boost includes
#include <boost/beast.hpp>
#include <boost/beast/http/impl/message.hpp>
#include <boost/lexical_cast.hpp>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include <awsmock/core/LogStream.h>
#include <awsmock/core/monitoring/MetricService.h>
#include <awsmock/dto/common/SecretsManagerClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/secretsmanager/SecretsManagerCmdHandler.h>
#include <awsmock/service/secretsmanager/SecretsManagerService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * AWS secrets manager mock handler.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerHandler : public AbstractHandler {

      public:

        /**
         * Constructor
         */
        explicit SecretsManagerHandler() = default;

        /**
         * HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return response HTTP response
         * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * Secrets manager module
         */
        Service::SecretsManagerService _secretsManagerService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_HANDLER_H
