//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/UnauthorizedException.h>

namespace AwsMock::Core {

    UnauthorizedException::UnauthorizedException(const http::status code, const char *resource, const char *requestId) : _code(code), _resource(resource), _requestId(requestId) {}

    UnauthorizedException::UnauthorizedException(const std::string &msg, const http::status code, const char *resource, const char *requestId) : _message(msg), _code(code), _resource(resource), _requestId(requestId) {}

    UnauthorizedException::UnauthorizedException(const UnauthorizedException &exc) = default;

    UnauthorizedException::~UnauthorizedException() noexcept = default;

    http::status UnauthorizedException::code() const noexcept { return _code; }

    std::string UnauthorizedException::message() const noexcept { return _message; }

    const char *UnauthorizedException::resource() const noexcept { return _resource; }

    const char *UnauthorizedException::requestId() const noexcept { return _requestId; }


}// namespace AwsMock::Core