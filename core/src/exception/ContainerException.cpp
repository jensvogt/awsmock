//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/ContainerException.h>

namespace AwsMock::Core {

    ContainerException::ContainerException(const ContainerException &exc) = default;

    ContainerException::~ContainerException() noexcept = default;

    ContainerException &ContainerException::operator=(const ContainerException &exc) = default;

} // namespace AwsMock::Core
