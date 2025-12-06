//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/ServiceException.h>

namespace AwsMock::Core {

    ServiceException::ServiceException(const http::status code) : _code(code) {}

    ServiceException::ServiceException(const std::string &msg, const http::status code) : _message(msg), _code(code) {}

    ServiceException::ServiceException(const ServiceException &exc) = default;

    ServiceException::~ServiceException() noexcept = default;

    http::status ServiceException::code() const noexcept { return _code; }

    std::string ServiceException::message() const noexcept { return _message; }

}// namespace AwsMock::Core