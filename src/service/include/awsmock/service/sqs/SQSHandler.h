//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_SQS_HANDLER_H
#define AWSMOCK_SERVICE_SQS_HANDLER_H

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/SQSClientCommand.h>
#include <awsmock/dto/sqs/DeleteMessageBatchEntry.h>
#include <awsmock/dto/sqs/DeleteMessageBatchRequest.h>
#include <awsmock/dto/sqs/GetQueueUrlRequest.h>
#include <awsmock/dto/sqs/GetQueueUrlResponse.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/sqs/SQSService.h>

#define DEFAULT_SQS_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief AWS SQS mock handler.
     *
     * <p>The SQS request are coming in two different flavours. Using the AWS CLI the queue URL is part of the HTTP parameters in the body of the message. Both are
     * using POST request, whereas the Java SDK is providing the queue-url as part of the HTTP URL in the header of the request.</p>
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSHandler final : public AbstractHandler {

      public:

        /**
         * @brief Constructor
         */
        explicit SQSHandler() : AbstractHandler("sqs-handler") {}

        /**
         * @brief HTTP POST request.
         *
         * @param request HTTP request
         * @param region AWS region
         * @param user AWS user
         * @return HTTP response
         * @see AbstractResource::handlePost(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &)
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
