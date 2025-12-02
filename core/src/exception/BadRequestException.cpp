//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/BadRequestException.h>

namespace AwsMock::Core {

    BadRequestException::BadRequestException(const BadRequestException &exc) = default;

    BadRequestException::~BadRequestException() noexcept = default;

    void BadRequestException::rethrow() const { throw *this; }

}// namespace AwsMock::Core