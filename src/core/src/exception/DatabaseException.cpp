//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/DatabaseException.h>

namespace AwsMock::Core {

    DatabaseException::DatabaseException(const http::status code, const char *resource, const char *requestId) : _code(code), _resource(resource), _requestId(requestId) {}

    DatabaseException::DatabaseException(const std::string &msg, const http::status code, const char *resource, const char *requestId) : _code(code), _message(msg), _resource(resource), _requestId(requestId) {}

    DatabaseException::DatabaseException(const DatabaseException &exc) = default;

    DatabaseException::~DatabaseException() noexcept = default;

    http::status DatabaseException::code() const noexcept { return _code; }

    std::string DatabaseException::message() const noexcept { return _message; }

    const char *DatabaseException::resource() const noexcept { return _resource; }

    const char *DatabaseException::requestId() const noexcept { return _requestId; }


}// namespace AwsMock::Core