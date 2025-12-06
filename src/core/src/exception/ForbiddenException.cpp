//
// Created by vogje01 on 02/09/2022.
//

#include "awsmock/core/exception/ForbiddenException.h"

namespace AwsMock::Core {

    ForbiddenException::ForbiddenException(const boost::beast::http::status code) : _code(code) {
    }

    ForbiddenException::ForbiddenException(const std::string &msg, const boost::beast::http::status code) : _message(msg), _code(code) {
    }

    ForbiddenException::ForbiddenException(const ForbiddenException &exc) = default;

    ForbiddenException::~ForbiddenException() noexcept = default;

    void ForbiddenException::rethrow() const { throw *this; }

}// namespace AwsMock::Core