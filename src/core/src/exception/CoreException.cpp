//
// Created by vogje01 on 02/09/2022.
//

#include "awsmock/core/exception/CoreException.h"

namespace AwsMock::Core {

    CoreException::CoreException(int code) : Poco::Exception(code) {}

    CoreException::CoreException(const std::string &msg, int code) : Poco::Exception(msg, code) {}

    CoreException::CoreException(const std::string &msg, const std::string &arg, int code) : Poco::Exception(msg, arg, code) {}

    CoreException::CoreException(const std::string &msg, const Poco::Exception &exc, int code) : Poco::Exception(msg, exc, code) {}

    CoreException::CoreException(const CoreException &exc) = default;

    CoreException::~CoreException() noexcept = default;

    CoreException &CoreException::operator=(const CoreException &exc) = default;

    const char *CoreException::name() const noexcept { return "CoreException: "; }
#ifndef _WIN32
    const char *CoreException::className() const noexcept { return typeid(*this).name(); }
#endif
    Core::CoreException *CoreException::clone() const { return new CoreException(*this); }

    void CoreException::rethrow() const { throw *this; }

}// namespace AwsMock::Core