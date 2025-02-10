//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_SERVICES_GATEWAY_SESSION_H
#define AWSMOCK_SERVICES_GATEWAY_SESSION_H

// C++ includes
#include <memory>
#include <queue>

// Boost includes
#include <boost/asio/dispatch.hpp>
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
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

// Maximal body size (10MB)
#define DEFAULT_MAX_BODY_SIZE (100 * 1024 * 1024)
#define DEFAULT_MAX_QUEUE_SIZE 250
#define DEFAULT_TIMEOUT 300

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
         * @brief HTTP session
         *
         * Takes ownership of the socket.
         *
         * @param socket
         */
        explicit GatewaySession(ip::tcp::socket &&socket);

        /**
         * @brief Start the session
         *
         * We need to be executing within a strand to perform async operations on the I/O objects in this session. Although not strictly necessary
         * for single-threaded contexts, this example code is written to be thread-safe by default.
         */
        void Run();

      private:

        /**
         * @brief Read callback
         */
        void DoRead();

        /**
         * @brief On read callback
         */
        void OnRead(const boost::beast::error_code &ec, std::size_t bytes_transferred);

        /**
         * @brief Queue write callback
         */
        void QueueWrite(http::message_generator response);

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
         * @brief Called to start/continue the write-loop.
         *
         * Should not be called when write_loop is already active.
         */
        void DoWrite();

        /**
         * @brief On read callback
         *
         * @param keep_alive keep alive flag
         * @param ec error code
         * @param bytes_transferred number of bytes transferred
         */
        void OnWrite(bool keep_alive, const boost::beast::error_code &ec, std::size_t bytes_transferred);

        /**
         * @brief On class callback
         */
        void DoShutdown();

        /**
         * @brief On clas callback
         */
        void DoClose();

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
        boost::beast::tcp_stream _stream;

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
         * HTTP request queue
         */
        std::queue<http::message_generator> _response_queue;

        /**
         * The parser is stored in an optional container so we can construct it from scratch it at the beginning of each new message.
         */
        boost::optional<http::request_parser<http::dynamic_body>> _parser;

        /**
         * Metric service
         */
        Monitoring::MetricService _metricService;
    };


    template<class SyncReadStream, class DynamicBuffer>
    void PrintChunkedBody(std::ostream &os, SyncReadStream &stream, DynamicBuffer &buffer, boost::beast::error_code &ec) {

        // Declare the parser with an empty body since
        // we plan on capturing the chunks ourselves.
        http::parser<true, http::empty_body> p;

        // This container will hold the extensions for each chunk
        http::chunk_extensions ce;

        // This string will hold the body of each chunk
        std::string chunk;

        // Declare our chunk header callback  This is invoked
        // after each chunk header and also after the last chunk.
        auto header_cb = [&](const std::uint64_t size, const boost::string_view extensions, boost::beast::error_code &ev) {
            // Parse the chunk extensions so we can access them easily
            ce.parse(extensions, ev);
            if (ev)
                return;

            // See if the chunk is too big
            if (size > (std::numeric_limits<std::size_t>::max)()) {
                ev = http::error::body_limit;
                return;
            }

            // Make sure we have enough storage, and
            // reset the container for the upcoming chunk
            chunk.reserve(size);
            chunk.clear();
        };

        // Set the callback. The function requires a non-const reference so we
        // use a local variable, since temporaries can only bind to const refs.
        p.on_chunk_header(header_cb);

        // Declare the chunk body callback. This is called one or
        // more times for each piece of a chunk body.
        auto body_cb = [&](const std::uint64_t remain, const boost::string_view body, boost::beast::error_code &ec) {
            // If this is the last piece of the chunk body, set the error so that the call to `read` returns,
            // and we can process the chunk.
            if (remain == body.size())
                ec = http::error::end_of_chunk;

            // Append this piece to our container
            chunk.append(body.data(), body.size());

            // The return value informs the parser of how much of the body we
            // consumed. We will indicate that we consumed everything passed in.
            return body.size();
        };
        p.on_chunk_body(body_cb);

        while (!p.is_done()) {
            // Read as much as we can. When we reach the end of the chunk, the chunk
            // body callback will make the read return with the end_of_chunk error.
            read(stream, buffer, p.get(), ec);
            if (!ec)
                continue;
            if (ec != http::error::end_of_chunk)
                return;
            ec.assign(0, ec.category());

            // We got a whole chunk, print the extensions:
            for (const auto &[fst, snd]: ce) {
                os << "Extension: " << fst;
                if (!snd.empty())
                    os << " = " << snd << std::endl;
                else
                    os << std::endl;
            }

            // Now print the chunk body
            os << "Chunk Body: " << chunk << std::endl;
        }

        // Get a reference to the parsed message, this is for convenience
        // Check each field promised in the "Trailer" header and output it
        for (auto const &msg = p.get(); auto const &name: http::token_list{msg[http::field::trailer]}) {
            // Find the trailer field
            auto it = msg.find(name);
            if (it == msg.end()) {
                // Oops! They promised the field but failed to deliver it
                os << "Missing Trailer: " << name << std::endl;
                continue;
            }
            os << it->name() << ": " << it->value() << std::endl;
        }
    }

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICES_GATEWAY_SESSION_H
