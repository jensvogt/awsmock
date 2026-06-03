//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <memory>

// Awsmock includes
#include "module/IModuleRepository.h"
#include "module/ModuleMongoRepository.h"


#include <awsmock/repository/cognito/CognitoMemoryRepository.h>
#include <awsmock/repository/cognito/CognitoMongoRepository.h>
#include <awsmock/repository/cognito/ICognitoRepository.h>
#include <awsmock/repository/module/IModuleRepository.h>
#include <awsmock/repository/module/ModuleMemoryRepository.h>
#include <awsmock/repository/module/ModuleMongoRepository.h>
#include <awsmock/repository/sns/ISNSRepository.h>
#include <awsmock/repository/sns/SNSMemoryRepository.h>
#include <awsmock/repository/sns/SNSMongoRepository.h>
#include <awsmock/repository/sqs/ISQSRepository.h>
#include <awsmock/repository/sqs/SQSMemoryRepository.h>
#include <awsmock/repository/sqs/SQSMongoRepository.h>

namespace Awsmock::Database {

    enum class BackendType { MONGODB,
                             MEMORY };

    class RepositoryFactory {

      public:

        static RepositoryFactory &instance() {
            static RepositoryFactory inst;
            return inst;
        }

        void initialize(const BackendType type) {
            _backend = type;
        }

        [[nodiscard]]
        std::shared_ptr<IModuleRepository> moduleRepository() const {
            static auto repo = createModuleRepository();
            return repo;
        }

        [[nodiscard]]
        std::shared_ptr<ISNSRepository> snsRepository() const {
            static auto repo = createSNSRepository();
            return repo;
        }

        [[nodiscard]]
        std::shared_ptr<ISQSRepository> sqsRepository() const {
            static auto repo = createSQSRepository();
            return repo;
        }

        [[nodiscard]]
        std::shared_ptr<ICognitoRepository> cognitoRepository() const {
            static auto repo = createCognitoRepository();
            return repo;
        }

      private:

        BackendType _backend = BackendType::MONGODB;

        [[nodiscard]]
        std::shared_ptr<IModuleRepository> createModuleRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<ModuleMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<ModuleMemoryRepository>();
            }
            return std::make_shared<ModuleMemoryRepository>();
        }

        [[nodiscard]]
        std::shared_ptr<ISNSRepository> createSNSRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<SNSMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<SNSMemoryRepository>();
            }
            return std::make_shared<SNSMemoryRepository>();
        }

        [[nodiscard]]
        std::shared_ptr<ISQSRepository> createSQSRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<SQSMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<SQSMemoryRepository>();
            }
            return std::make_shared<SQSMemoryRepository>();
        }

        [[nodiscard]]
        std::shared_ptr<ICognitoRepository> createCognitoRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<CognitoMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<CognitoMemoryRepository>();
            }
            return std::make_shared<CognitoMemoryRepository>();
        }
    };

}// namespace Awsmock::Database