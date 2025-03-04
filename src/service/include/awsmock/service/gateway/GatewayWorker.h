//
// Created by vogje01 on 12/17/24.
//

#ifndef AWSMOCK_SERVICE_GATEWAY_HTTP_WORKER_H
#define AWSMOCK_SERVICE_GATEWAY_HTTP_WORKER_H

// C++ standard includes
#include <string>
#include <utility>

// Boost includes
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_static_buffer.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/serializer.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/numeric/ublas/fwd.hpp>

// AwsMock includes
#include <awsmock/core/FieldAlloc.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/service/cognito/CognitoHandler.h>
#include <awsmock/service/common/AbstractHandler.h>
#include <awsmock/service/dynamodb/DynamoDbHandler.h>
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

namespace AwsMock::Service {

    namespace beast = boost::beast;  // from <boost/beast.hpp>
    namespace http = beast::http;    // from <boost/beast/http.hpp>
    namespace net = boost::asio;     // from <boost/asio.hpp>
    using tcp = boost::asio::ip::tcp;// from <boost/asio/ip/tcp.hpp>

    typedef std::map<std::string, std::shared_ptr<AbstractHandler>> RoutingTable;

    class GatewayWorker {

      public:

        /**
         * @brief Constructor
         *
         * @param acceptor request acceptor
         * @param docRoot document root directory
         */
        GatewayWorker(tcp::acceptor &acceptor, std::string docRoot);

        /**
         * @brief Copy constructor
         *
         * @param worker worker to copy from
         */
        GatewayWorker(GatewayWorker const &worker) = delete;

        /**
         * @brief Assignment constructor
         *
         * @param worker worker to assign from
         * @return assigned worker
         */
        GatewayWorker &operator=(GatewayWorker const &worker) = delete;

        /**
         * @brief Start the server
         */
        void Start();

      private:

        using alloc_t = fields_alloc<char>;

        using request_body_t = http::string_body;

        void Accept();

        void ReadRequest();

        void ProcessRequest(http::request<request_body_t, http::basic_fields<alloc_t>> const &request);

        void SendBadResponse(http::status status, std::string const &error);

        void SendOkResponse();

        void SendOptionsResponse();

        void SendFile(beast::string_view target);

        /**
         * @brief Returns the authorization header
         *
         * @param request HTTP request
         * @param secretAccessKey AWS secret access key
         * @return AuthorizationHeaderKeys
         * @see AuthorizationHeaderKeys
         */
        static Core::AuthorizationHeaderKeys GetAuthorizationKeys(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &secretAccessKey);

        void CheckDeadline();

        // The acceptor used to listen for incoming connections.
        tcp::acceptor &_acceptor;

        // The path to the root of the document directory.
        std::string _docRoot;

        // The socket for the currently connected client.
        tcp::socket _socket{_acceptor.get_executor()};

        // The buffer for performing reads
        beast::flat_static_buffer<8192> _buffer;

        // The allocator used for the fields in the request and reply.
        alloc_t _alloc{8192};

        // The parser for reading the requests
        boost::optional<http::request_parser<request_body_t, alloc_t>> _gatewayParser;

        // The timer putting a time limit on requests.
        net::steady_timer _requestDeadline{_acceptor.get_executor(), (std::chrono::steady_clock::time_point::max) ()};

        // The string-based response message.
        boost::optional<http::response<http::string_body, http::basic_fields<alloc_t>>> _stringResponse;

        // The string-based response serializer.
        boost::optional<http::response_serializer<http::string_body, http::basic_fields<alloc_t>>> _stringSerializer;

        // The file-based response message.
        boost::optional<http::response<http::file_body, http::basic_fields<alloc_t>>> _fileResponse;

        // The file-based response serializer.
        boost::optional<http::response_serializer<http::file_body, http::basic_fields<alloc_t>>> _fileSerializer;

        /**
         * Routine table
         */
        RoutingTable _routingTable;

        /**
         * Verify signature flag
         */
        bool _verifySignature;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_GATEWAY_HTTP_WORKER_H
