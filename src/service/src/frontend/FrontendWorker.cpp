//
// Created by vogje01 on 12/17/24.
//

#include <awsmock/service/frontend/FrontendWorker.h>

namespace AwsMock::Service::Frontend {

    void FrontendWorker::Start() {
        Accept();
        CheckDeadline();
    }

    void FrontendWorker::Accept() {

        int timeout = Core::Configuration::instance().GetValueInt("awsmock.frontend.timeout");

        // Clean up any previous connection.
        beast::error_code ec = _socket.close(ec);
        if (ec) {
            log_error << "Close socket failed: " << ec.message();
        }
        _buffer.consume(_buffer.size());

        _acceptor.async_accept(_socket, [this, timeout](const beast::error_code &errorCode) {
            if (errorCode) {
                Accept();
            } else {
                // Request must be fully processed within timeout seconds.
                _requestDeadline.expires_after(std::chrono::seconds(timeout));

                ReadRequest();
            }
        });
    }

    void FrontendWorker::ReadRequest() {
        // On each read the parser needs to be destroyed and recreated. We store it in a boost::optional to achieve that.
        //
        // Arguments passed to the parser constructor are forwarded to the message object. A single argument is forwarded
        // to the body constructor.
        //
        // We construct the dynamic body with a 1MB limit to prevent vulnerability to buffer attacks.
        //
        _parser.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));

        // No limit
        _parser->body_limit(boost::none);

        async_read(_socket, _buffer, *_parser, [this](const beast::error_code &ec, std::size_t) {
            if (ec)
                Accept();
            else
                ProcessRequest(_parser->get());
        });
    }

    void FrontendWorker::ProcessRequest(http::request<request_body_t, http::basic_fields<alloc_t>> const &req) {
        switch (req.method()) {

            case http::verb::get:
                SendFile(req.target());
                break;

            default:
                // We return responses indicating an error if we do not recognize the request method.
                SendBadResponse(http::status::bad_request, "Invalid request-method '" + std::string(req.method_string()) + "'\r\n");
                break;
        }
    }

    void FrontendWorker::SendBadResponse(const http::status status, std::string const &error) {

        _stringResponse.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));
        _stringResponse->result(status);
        _stringResponse->keep_alive(false);
        _stringResponse->set(http::field::server, "Beast");
        _stringResponse->set(http::field::content_type, "text/plain");
        _stringResponse->body() = error;
        _stringResponse->prepare_payload();
        _stringSerializer.emplace(*_stringResponse);

        http::async_write(_socket, *_stringSerializer, [this](beast::error_code ec, std::size_t) {
            ec = _socket.shutdown(tcp::socket::shutdown_send, ec);
            if (ec) {
                log_error << "Shutdown socket failed: " << ec.message();
            }
            _stringSerializer.reset();
            _stringResponse.reset();
            Accept();
        });
    }

    void FrontendWorker::SendFile(beast::string_view target) {

        // Request path must be absolute and not contain "..".
        if (target.empty() || target[0] != '/' || target.find("..") != std::string::npos) {
            log_error << "File not found, path: " << std::string(target);
            SendBadResponse(http::status::not_found, "File not found\r\n");
            return;
        }

        std::string full_path = _docRoot;
        if (target.size() == 1) {
            target = boost::string_view(DEFAULT_PAGE.c_str(), DEFAULT_PAGE.length());
        }
        full_path.append(target.data(), target.size());
        log_debug << "Serving: " << full_path;

        http::file_body::value_type file;
        beast::error_code ec;
        if (boost::filesystem::exists(full_path)) {
            file.open(full_path.c_str(), beast::file_mode::read, ec);
        } else {
            full_path = _docRoot + "/index.html";
            file.open(full_path.c_str(), beast::file_mode::read, ec);
            log_trace << full_path << " MIME: " << mime_type(target);
        }
        _fileResponse.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));
        _fileResponse->result(http::status::ok);
        _fileResponse->keep_alive(false);
        _fileResponse->set(http::field::server, "Beast");
        _fileResponse->set(http::field::content_type, mime_type(target));
        _fileResponse->body() = std::move(file);
        _fileResponse->prepare_payload();
        _fileSerializer.emplace(*_fileResponse);

        http::async_write(_socket, *_fileSerializer, [this](beast::error_code errorCode, std::size_t) {
            errorCode = _socket.shutdown(tcp::socket::shutdown_send, errorCode);
            if (errorCode) {
                log_error << "Shutdown socket failed: " << errorCode.message();
            }
            _fileSerializer.reset();
            _fileResponse.reset();
            Accept();
        });
    }

    void FrontendWorker::CheckDeadline() {
        // The deadline may have moved, so check it has really passed.
        if (_requestDeadline.expiry() <= std::chrono::steady_clock::now()) {
            // Close socket to cancel any outstanding operation.
            _socket.close();

            // Sleep indefinitely until we're given a new deadline.
            _requestDeadline.expires_at((std::chrono::steady_clock::time_point::max)());
        }

        _requestDeadline.async_wait([this](beast::error_code) {
            CheckDeadline();
        });
    }

};// namespace AwsMock::Service::Frontend
