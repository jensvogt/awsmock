//
// Created by vogje01 on 23/07/2023.
//

#include <awsmock/utils/TestUtils.h>

namespace AwsMock::Database {

    std::vector<std::string> TestUtils::_modules = {"s3", "sqs", "sns", "lambda", "transfer", "cognito", "gateway", "database", "kms", "dynamodb"};

    void TestUtils::CreateServices() {
        ModuleDatabase &_serviceDatabase = ModuleDatabase::instance();
        for (const auto &it: _modules) {
            _serviceDatabase.CreateOrUpdateModule({.oid = {}, .name = it, .state = Entity::Module::ModuleState::RUNNING, .status = Entity::Module::ModuleStatus::ACTIVE});
        }
    }
}// namespace AwsMock::Database