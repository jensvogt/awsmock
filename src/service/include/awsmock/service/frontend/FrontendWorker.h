//
// Created by vogje01 on 12/17/24.
//

#ifndef AWSMOCK_SERVICE_FRONTEND_HTTP_WORKER_H
#define AWSMOCK_SERVICE_FRONTEND_HTTP_WORKER_H

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
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/FieldAlloc.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/config/Configuration.h>

namespace beast = boost::beast;  // from <boost/beast.hpp>
namespace http = beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;     // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;// from <boost/asio/ip/tcp.hpp>

#define DEFAULT_PAGE std::string("/index.html")

namespace AwsMock::Service::Frontend {

    class FrontendWorker {

      public:

        /**
         * @brief Constructor
         *
         * @param acceptor request acceptor
         * @param docRoot document root directory
         */
        FrontendWorker(boost::asio::ip::tcp::acceptor &acceptor, std::string docRoot) : _acceptor(acceptor), _docRoot(std::move(docRoot)) {}

        /**
         * @brief Copy constructor
         *
         * @param worker worker to copy from
         */
        FrontendWorker(FrontendWorker const &worker) = delete;

        /**
         * @brief Assignment constructor
         *
         * @param worker worker to assign from
         * @return assigned worker
         */
        FrontendWorker &operator=(FrontendWorker const &worker) = delete;

        /**
         * @brief Start the server
         */
        void Start();

      private:

        using alloc_t = fields_alloc<char>;
        using request_body_t = http::string_body;

        /**
         * @brief Accept incoming socket request
         */
        void Accept();

        /**
         * @brief Read incoming socket request
         */
        void ReadRequest();

        /**
         * @brief Process incoming socket request
         */
        void ProcessRequest(http::request<request_body_t, http::basic_fields<alloc_t>> const &req);

        /**
         * @brief Sends a bad request response
         *
         * @param status HTTP status
         * @param error error message
         */
        void SendBadResponse(http::status status, std::string const &error);

        /**
         * @brief Handle preflight requests
         *
         * @param request incoming HTTP request
         */
        void HandleOptionsRequest(const http::request<request_body_t, http::basic_fields<alloc_t>> &request);

        /**
         * @brief Handle file request
         *
         * @param target
         */
        void SendFile(beast::string_view target);

        /**
         * @brief Check request deadline
         */
        void CheckDeadline();

        /**
         * The acceptor used to listen for incoming connections.
         */
        tcp::acceptor &_acceptor;

        /**
         * The path to the root of the document directory.
         */
        std::string _docRoot;

        /**
         * The socket for the currently connected client.
         */
        tcp::socket _socket{_acceptor.get_executor()};

        /**
         * The buffer for performing reads
         */
        beast::flat_static_buffer<8192> _buffer;

        /**
         * The allocator used for the fields in the request and reply.
         */
        alloc_t _alloc{8192};

        /**
         * The parser for reading the requests
         */
        boost::optional<http::request_parser<request_body_t, alloc_t>> _parser;

        /**
         * The timer putting a time limit on requests.
         */
        net::steady_timer _requestDeadline{_acceptor.get_executor(), (std::chrono::steady_clock::time_point::max)()};

        /**
         * The string-based response message.
         */
        boost::optional<http::response<http::string_body, http::basic_fields<alloc_t>>> _stringResponse;

        /**
         * The string-based response serializer.
         */
        boost::optional<http::response_serializer<http::string_body, http::basic_fields<alloc_t>>> _stringSerializer;

        /**
         * The file-based response message.
         */
        boost::optional<http::response<http::file_body, http::basic_fields<alloc_t>>> _fileResponse;

        /**
         * The file-based response serializer.
         */
        boost::optional<http::response_serializer<http::file_body, http::basic_fields<alloc_t>>> _fileSerializer;
    };

}// namespace AwsMock::Service::Frontend

#endif// AWSMOCK_SERVICE_FRONTEND_HTTP_WORKER_H
