//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/NotFoundException.h>

namespace AwsMock::Core {

    NotFoundException::NotFoundException(const http::status code, const char *resource, const char *requestId) : _code(code), _resource(resource), _requestId(requestId) {}

    NotFoundException::NotFoundException(const std::string &msg, const http::status code, const char *resource, const char *requestId) : _message(msg), _code(code), _resource(resource), _requestId(requestId) {}

    NotFoundException::NotFoundException(const NotFoundException &exc) = default;

    NotFoundException::~NotFoundException() noexcept = default;

    http::status NotFoundException::code() const noexcept { return _code; }

    std::string NotFoundException::message() const noexcept { return _message; }

    const char *NotFoundException::resource() const noexcept { return _resource; }

    const char *NotFoundException::requestId() const noexcept { return _requestId; }

}// namespace AwsMock::Core