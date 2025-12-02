//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Core {

    JsonException::JsonException(const http::status code, const char *resource, const char *requestId) : _code(code), _resource(resource), _requestId(requestId) {}

    JsonException::JsonException(const std::string &msg, const http::status code, const char *resource, const char *requestId) : _code(code), _message(msg), _resource(resource), _requestId(requestId) {}

    JsonException::JsonException(const JsonException &exc) = default;

    JsonException::~JsonException() noexcept = default;

    http::status JsonException::code() const noexcept { return _code; }

    std::string JsonException::message() const noexcept { return _message; }

    const char *JsonException::resource() const noexcept { return _resource; }

    const char *JsonException::requestId() const noexcept { return _requestId; }

}// namespace AwsMock::Core