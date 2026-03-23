//
// Created by vogje01 on 02/09/2022.
//

#include <awsmock/core/exception/DynamoDbParseException.h>

namespace AwsMock::Core {

    DynamoDbParseException::DynamoDbParseException(const DynamoDbParseException &exc) = default;

    DynamoDbParseException::~DynamoDbParseException() noexcept = default;

    void DynamoDbParseException::rethrow() const { throw *this; }

}// namespace AwsMock::Core