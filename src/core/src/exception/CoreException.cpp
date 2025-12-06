//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/CoreException.h>

namespace AwsMock::Core {

    CoreException::CoreException(const CoreException &exc) = default;

    CoreException::~CoreException() noexcept = default;

    CoreException &CoreException::operator=(const CoreException &exc) = default;

}// namespace AwsMock::Core