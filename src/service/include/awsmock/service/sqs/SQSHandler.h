//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_SQS_HANDLER_H
#define AWSMOCK_SERVICE_SQS_HANDLER_H

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/dto/common/SQSClientCommand.h>
#include <awsmock/dto/sqs/DeleteMessageBatchRequest.h>
#include <awsmock/dto/sqs/GetQueueUrlRequest.h>
#include <awsmock/dto/sqs/GetQueueUrlResponse.h>
#include <awsmock/dto/sqs/internal/ListDefaultMessageAttributeCountersRequest.h>
#include <awsmock/dto/sqs/internal/ListMessagesRequest.h>
#include <awsmock/dto/sqs/internal/ListMessagesResponse.h>
#include <awsmock/dto/sqs/model/DeleteMessageBatchEntry.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/sqs/SQSService.h>
#include <boost/asio/detached.hpp>
#include <boost/asio/spawn.hpp>

#define DEFAULT_SQS_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief AWS SQS mock handler.
     *
     * @par
     * The SQS request are coming in two different flavors. Using the AWS CLI, the queue URL is part of the HTTP parameters in the body of the message. Both are
     * using POST request, whereas the Java SDK is providing the queue-url as part of the HTTP URL in the header of the request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit SQSHandler(boost::asio::io_context &ioc) : AbstractHandler("sqs-handler", ioc), _sqsService(ioc) {}

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
         * Get the queue userAttributes.
         *
         * @param payload HTTP body
         * @return list of queue userAttributes
         */
        static std::vector<Dto::SQS::QueueAttribute> GetQueueAttributes(const std::string &payload);

        /**
         * Get the queue tags.
         *
         * @param payload HTTP body
         * @return list of queue tags
         */
        static std::map<std::string, std::string> GetQueueTags(const std::string &payload);

        /**
         * Get the queue attribute names.
         *
         * @param payload HTTP body
         * @return list of queue attribute names
         */
        static std::vector<std::string> GetQueueAttributeNames(const std::string &payload);

        /**
         * Get the message attributes.
         *
         * @param payload HTTP body
         * @return list of message userAttributes
         */
        static std::map<std::string, Dto::SQS::MessageAttribute> GetMessageAttributes(const std::string &payload);

        /**
         * SQS service
         */
        SQSService _sqsService;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SQS_HANDLER_H
