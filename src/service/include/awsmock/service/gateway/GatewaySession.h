//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_SERVICES_GATEWAY_SESSION_H
#define AWSMOCK_SERVICES_GATEWAY_SESSION_H

// C++ includes
#include <memory>
#include <queue>

// Boost includes
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/service/cognito/CognitoHandler.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/dynamodb/DynamoDbHandler.h>
#include <awsmock/service/gateway/GatewayRouter.h>
#include <awsmock/service/kms/KMSHandler.h>
#include <awsmock/service/lambda/LambdaHandler.h>
#include <awsmock/service/module/ModuleHandler.h>
#include <awsmock/service/monitoring/MonitoringHandler.h>
#include <awsmock/service/s3/S3Handler.h>
#include <awsmock/service/secretsmanager/SecretsManagerHandler.h>
#include <awsmock/service/sns/SNSHandler.h>
#include <awsmock/service/sqs/SQSHandler.h>
#include <awsmock/service/ssm/SSMHandler.h>
#include <awsmock/service/transfer/TransferHandler.h>
#include <boost/asio/spawn.hpp>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief HTTP session manager
     *
     * @par
     * From: https://www.boost.org/doc/libs/1_86_0/libs/beast/example/advanced/server/advanced_server.cpp
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class GatewaySession : public std::enable_shared_from_this<GatewaySession> {

      public:

        /**
         * @brief Constructor
         */
        explicit GatewaySession();

        /**
         * @brief Start the session
         *
         * We need to be executing within a strand to perform async operations on the I/O objects in this session. Although not strictly necessary
         * for single-threaded contexts, this example code is written to be thread-safe by default.
         */
        void DoSession(boost::beast::tcp_stream &stream, const boost::asio::yield_context &yield);

      private:

        /**
         * @brief Return a response for the given request.
         *
         * The concrete type of the response message (which depends on the request), is type-erased in message_generator.
         *
         * @tparam Body HTTP body
         * @tparam Allocator allocator
         * @param request HTTP request
         * @return
         */
        template<class Body, class Allocator>
        http::message_generator HandleRequest(http::request<Body, http::basic_fields<Allocator>> &&request);

        /**
         * @brief Returns the authorization header
         *
         * @param request HTTP request
         * @param secretAccessKey AWS secret access key
         * @return AuthorizationHeaderKeys
         * @see AuthorizationHeaderKeys
         */
        static Core::AuthorizationHeaderKeys GetAuthorizationKeys(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey);

        /**
         * @brief Handles options request
         *
         * @param request HTTP request
         * @return options response
         */
        static http::response<http::dynamic_body> HandleOptionsRequest(const http::request<http::dynamic_body> &request);

        /**
         * @brief Handles continue request (HTTP status: 100)
         *
         * @param _stream HTTP socket stream
         */
        static void HandleContinueRequest(boost::beast::tcp_stream &_stream);

        /**
         * TCP stream
         */
        //boost::beast::tcp_stream _stream;

        /**
         * Read buffer
         */
        boost::beast::flat_buffer _buffer;

        /**
         * Queue limit
         */
        std::size_t _queueLimit;

        /**
         * Mox body size
         */
        long _bodyLimit;

        /**
         * Request timeout
         */
        int _timeout;

        /**
         * Verify signature flag
         */
        bool _verifySignature;

        /**
         * Default region
         */
        std::string _region;

        /**
         * Default user
         */
        std::string _user;

        /**
         * HTTP request queue
         */
        //std::queue<http::message_generator> _response_queue;

        /**
         * The parser is stored in an optional container, so we can construct it from scratch it at the beginning of each new message.
         */
        //boost::optional<http::request_parser<http::dynamic_body>> _parser;

        /**
         * Metric service
         */
        Monitoring::MetricService _metricService = Monitoring::MetricService::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICES_GATEWAY_SESSION_H
