// Awsmock includes
#include <SqsServer.h>

#include "RepositoryFactory.h"

namespace Awsmock::sqs {

    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace asio = boost::asio;
    namespace local = asio::local;

    // ── Helpers ──────────────────────────────────────────────────────────────

    // Parses "key=value&key2=value2" into a map. Values are not URL-decoded
    // here — extend if full percent-decoding is needed.
    static std::unordered_map<std::string, std::string> parseForm(std::string_view body) {
        std::unordered_map<std::string, std::string> params;
        while (!body.empty()) {
            const auto amp = body.find('&');
            const auto pair = body.substr(0, amp);
            if (const auto eq = pair.find('='); eq != std::string_view::npos) params.emplace(std::string(pair.substr(0, eq)), std::string(pair.substr(eq + 1)));
            if (amp == std::string_view::npos) break;
            body.remove_prefix(amp + 1);
        }
        return params;
    }

    static std::string requestId() {
        // Minimal pseudo-random request ID — replace with a real UUID generator.
        return Core::UuidUtils::CreateRandomUuid();
    }

    static http::response<http::string_body> jsonResponse(const http::request<http::string_body> &req, const http::status status, std::string body) {
        http::response<http::string_body> res{status, req.version()};
        res.set(http::field::content_type, "application/json");
        res.keep_alive(req.keep_alive());
        res.body() = std::move(body);
        res.prepare_payload();
        return res;
    }

    static http::response<http::string_body> xmlResponse(const http::request<http::string_body> &req, const http::status status, std::string body) {

        http::response<http::string_body> res{status, req.version()};
        res.set(http::field::content_type, "text/xml");
        res.keep_alive(req.keep_alive());
        res.body() = std::move(body);
        res.prepare_payload();
        return res;
    }

    static http::response<http::string_body> errorResponse(const http::request<http::string_body> &req, const std::string_view code, const std::string_view message) {

        const std::string body =
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<ErrorResponse xmlns=\"http://queue.amazonaws.com/doc/2012-11-05/\">"
                "<Error><Type>Sender</Type><Code>" + std::string(code) + "</Code>"
                "<Message>" + std::string(message) + "</Message></Error>"
                "<RequestId>" + requestId() + "</RequestId>"
                "</ErrorResponse>";
        return xmlResponse(req, http::status::bad_request, body);
    }

    // ── Action handlers ──────────────────────────────────────────────────────
    // Each handler receives the parsed form parameters and the original request.
    // Return a fully formed HTTP response.

    static http::response<http::string_body> handleCreateQueue(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueName");
        const std::string name = it != params.end() ? it->second : "(unknown)";
        log_info << "SQS CreateQueue name=" << name;

        Database::Entity::SQS::Queue queue;
        queue.name = name;
        queue.url = Core::SqsUtils::createQueueUrl(name);
        queue.arn = Core::SqsUtils::createQueueArn(name);

        const auto saved = Database::RepositoryFactory::instance().sqsRepository()->upsertQueue(queue);

        const boost::json::object body{
                {"QueueUrl", saved.url},
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleDeleteQueue(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueUrl");
        log_info << "SQS DeleteQueue url=" << (it != params.end() ? it->second : "(unknown)");
        if (it != params.end()) {
            const std::string url = it->second;
            const std::string name = Core::SqsUtils::extractQueueName(url);
            log_info << "SQS DeleteQueue name=" << name;
            Database::RepositoryFactory::instance().sqsRepository()->removeQueueByName(name);
        }
        const boost::json::object body{
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleGetQueueUrl(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueName");
        const std::string name = it != params.end() ? it->second : "(unknown)";
        log_info << "SQS GetQueueUrl, name: " << name;

        const std::optional<Database::Entity::SQS::Queue> queue = Database::RepositoryFactory::instance().sqsRepository()->findQueueByName(name);
        log_debug << "Got SQS queue, name: " << name << ", url: " << (queue.has_value() ? queue->url : "(none)");

        const boost::json::object body{
                {"QueueUrl", queue.has_value() ? queue->url : ""},
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleListQueues(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        log_info << "SQS ListQueues";

        // Get the queue list from the DB
        const std::vector<Database::Entity::SQS::Queue> queues = Database::RepositoryFactory::instance().sqsRepository()->findAllQueues();
        log_info << "Got queue list, count: " << queues.size();

        boost::json::array urlArray;
        for (const auto &q: queues) {
            urlArray.emplace_back(q.url);
        }

        const boost::json::object body{
                {"QueueUrls", urlArray},
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleSendMessage(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueUrl");
        log_info << "SQS SendMessage url=" << (it != params.end() ? it->second : "(unknown)");

        const boost::json::object body{
                {"MessageId", "00000000-0000-0000-0000-000000000000"},
                {"MD5OfMessageBody", "d41d8cd98f00b204e9800998ecf8427e"},
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleReceiveMessage(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueUrl");
        log_info << "SQS ReceiveMessage url=" << (it != params.end() ? it->second : "(unknown)");

        const boost::json::object body{
                {"Messages", boost::json::array{}},
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleDeleteMessage(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueUrl");
        log_info << "SQS DeleteMessage url=" << (it != params.end() ? it->second : "(unknown)");

        const boost::json::object body{
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handlePurgeQueue(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueUrl");
        log_info << "SQS PurgeQueue url=" << (it != params.end() ? it->second : "(unknown)");

        const boost::json::object body{
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleGetQueueAttributes(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueUrl");
        log_info << "SQS GetQueueAttributes url=" << (it != params.end() ? it->second : "(unknown)");

        const boost::json::object body{
                {"Attributes", boost::json::object{}},
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    static http::response<http::string_body> handleSetQueueAttributes(const http::request<http::string_body> &req, const std::unordered_map<std::string, std::string> &params) {

        const auto it = params.find("QueueUrl");
        log_info << "SQS SetQueueAttributes url=" << (it != params.end() ? it->second : "(unknown)");

        const boost::json::object body{
                {"ResponseMetadata", boost::json::object{{"RequestId", requestId()}}}};
        return jsonResponse(req, http::status::ok, boost::json::serialize(body));
    }

    // ── Request dispatcher ───────────────────────────────────────────────────

    static http::response<http::string_body> dispatch(const http::request<http::string_body> &req) {

        // Debugging
        //Core::dumpRequest(req);

        // Merge body params and URL query-string params. Query string wins for Action
        // because AWS CLI / SDKs often send: POST /?Action=ListQueues with empty body.
        // JSON body (application/x-amz-json-1.0) is flattened into the same params map.
        auto params = parseForm(req.body());
        if (const auto contentType = std::string(req[http::field::content_type]); contentType.find("json") != std::string::npos && !req.body().empty()) {
            boost::system::error_code ec;
            if (const auto jv = boost::json::parse(req.body(), ec); !ec && jv.is_object()) {
                for (const auto &[k, v]: jv.as_object()) {
                    if (v.is_string()) params.insert_or_assign(std::string(k), std::string(v.as_string()));
                }
            }
        }

        const auto target = std::string(req.target());
        if (const auto q = target.find('?'); q != std::string::npos) for (auto &[k, v]: parseForm(std::string_view(target).substr(q + 1))) params.insert_or_assign(std::move(k), std::move(v));

        // Fall back to X-Amz-Target header: "AmazonSQS.ListQueues" → "ListQueues"
        if (!params.contains("Action")) {
            if (const auto hdr = std::string(req["X-Amz-Target"]); !hdr.empty()) {
                if (const auto dot = hdr.rfind('.'); dot != std::string::npos) params["Action"] = hdr.substr(dot + 1);
            }
        }

        const auto actionIt = params.find("Action");
        if (actionIt == params.end()) {
            log_error << "MissingAction: target=" << target << " body=" << req.body();
            return errorResponse(req, "MissingAction", "The request must contain Action.");
        }

        const auto &action = actionIt->second;
        log_debug << "SQS action=" << action;

        if (action == "CreateQueue") return handleCreateQueue(req, params);
        if (action == "DeleteQueue") return handleDeleteQueue(req, params);
        if (action == "GetQueueUrl") return handleGetQueueUrl(req, params);
        if (action == "ListQueues") return handleListQueues(req, params);
        if (action == "SendMessage") return handleSendMessage(req, params);
        if (action == "ReceiveMessage") return handleReceiveMessage(req, params);
        if (action == "DeleteMessage") return handleDeleteMessage(req, params);
        if (action == "PurgeQueue") return handlePurgeQueue(req, params);
        if (action == "GetQueueAttributes") return handleGetQueueAttributes(req, params);
        if (action == "SetQueueAttributes") return handleSetQueueAttributes(req, params);

        return errorResponse(req, "InvalidAction", "Action not implemented: " + action);
    }

    // ── SqsSession ───────────────────────────────────────────────────────────
    // Handles one Unix-socket connection: reads requests in a loop, dispatches,
    // writes responses.

    class SqsSession : public std::enable_shared_from_this<SqsSession> {
    public:

        explicit SqsSession(local::stream_protocol::socket sock) : _stream(std::move(sock)) {}

        void run() { doRead(); }

    private:

        void doRead() {
            _req = {};
            http::async_read(_stream, _buf, _req, [self = shared_from_this()](beast::error_code ec, std::size_t) {
                if (ec) return;
                self->doWrite(dispatch(self->_req));
            });
        }

        void doWrite(http::response<http::string_body> res) {
            const bool keepAlive = res.keep_alive();
            auto sp = std::make_shared<http::response<http::string_body> >(std::move(res));
            http::async_write(_stream, *sp, [self = shared_from_this(), sp, keepAlive](beast::error_code ec, std::size_t) {
                if (!ec && keepAlive) self->doRead();
            });
        }

        // beast generic stream wrapper for a local socket
        beast::basic_stream<local::stream_protocol> _stream;
        beast::flat_buffer _buf;
        http::request<http::string_body> _req;
    };

    // ── SqsServer ────────────────────────────────────────────────────────────

    SqsServer::SqsServer(std::string socketPath, const int threads) : _socketPath(std::move(socketPath)), _ioc(threads), _acceptor(_ioc), _threads(threads) {

        unlink(_socketPath.c_str());

        const local::stream_protocol::endpoint ep(_socketPath);
        _acceptor.open(ep.protocol());
        _acceptor.bind(ep);
        _acceptor.listen(asio::socket_base::max_listen_connections);

        log_info << "SQS service listening on " << _socketPath;
    }

    void SqsServer::start() {
        doAccept();
        _workers.reserve(_threads);
        for (int i = 0; i < _threads; ++i) _workers.emplace_back([this] { _ioc.run(); });
    }

    void SqsServer::stop() {
        _ioc.stop();
        for (auto &t: _workers) if (t.joinable()) t.join();
        ::unlink(_socketPath.c_str());
        log_info << "SQS service stopped";
    }

    void SqsServer::doAccept() {
        _acceptor.async_accept([this](const beast::error_code &ec, local::stream_protocol::socket sock) {
            if (!ec) std::make_shared<SqsSession>(std::move(sock))->run();
            doAccept();
        });
    }

}// namespace Awsmock::sqs
