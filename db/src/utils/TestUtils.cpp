//
// Created by vogje01 on 23/07/2023.
//

#include <awsmock/utils/TestUtils.h>

namespace Awsmock::Database {

    std::vector<std::string> TestUtils::_modules = {"s3", "sqs", "sns", "lambda", "transfer", "cognito", "gateway", "database", "kms", "dynamodb"};

    void TestUtils::CreateServices() {
        const std::shared_ptr<IModuleRepository> _serviceDatabase = RepositoryFactory::instance().moduleRepository();
        for (const auto &it: _modules) {
            Entity::Module::Module module = {.oid = {}, .name = it, .state = Entity::Module::ModuleState::RUNNING, .status = Entity::Module::ModuleStatus::ACTIVE};
            module = _serviceDatabase->createOrUpdateModule(module);
            //log_debug << "Created module: " << module.name;
        }
    }
}// namespace Awsmock::Database