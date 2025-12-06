//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_DYNAMODB_HANDLER_H
#define AWSMOCK_SERVICE_DYNAMODB_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/DynamoDbClientCommand.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief AWS DynamoDB mock handler
     *
     * AWS DynamoDB HTTP request handler. All DynamoDB related REST call are ending here.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit DynamoDbHandler(boost::asio::io_context &ioc) : AbstractHandler("dynamodb-handler", ioc) {}

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region name
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::HandlePostRequest
         */
        http::response<http::dynamic_body> HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) override;

      private:

        /**
         * DynamoDB service
         */
        DynamoDbService _dynamoDbService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_DYNAMODB_HANDLER_H
