//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <memory>

// Awsmock includes
#include <awsmock/repository/cognito/CognitoMemoryRepository.h>
#include <awsmock/repository/cognito/CognitoMongoRepository.h>
#include <awsmock/repository/cognito/ICognitoRepository.h>
#include <awsmock/repository/dynamodb/DynamoDbMemoryRepository.h>
#include <awsmock/repository/dynamodb/DynamoDbMongoRepository.h>
#include <awsmock/repository/dynamodb/IDynamoDbRepository.h>
#include <awsmock/repository/kms/IKMSRepository.h>
#include <awsmock/repository/kms/KMSMemoryRepository.h>
#include <awsmock/repository/kms/KMSMongoRepository.h>
#include <awsmock/repository/module/IModuleRepository.h>
#include <awsmock/repository/module/ModuleMemoryRepository.h>
#include <awsmock/repository/module/ModuleMongoRepository.h>
#include <awsmock/repository/monitoring/IMonitoringRepository.h>
#include <awsmock/repository/monitoring/MonitoringMemoryRepository.h>
#include <awsmock/repository/monitoring/MonitoringMongoRepository.h>
#include <awsmock/repository/sns/ISNSRepository.h>
#include <awsmock/repository/sns/SNSMemoryRepository.h>
#include <awsmock/repository/sns/SNSMongoRepository.h>
#include <awsmock/repository/sqs/ISQSRepository.h>
#include <awsmock/repository/sqs/SQSMemoryRepository.h>
#include <awsmock/repository/sqs/SQSMongoRepository.h>
#include <awsmock/repository/ssm/ISSMRepository.h>
#include <awsmock/repository/ssm/SSMMemoryRepository.h>
#include <awsmock/repository/ssm/SSMMongoRepository.h>
#include <awsmock/repository/transfer/ITransferRepository.h>
#include <awsmock/repository/transfer/TransferMemoryRepository.h>
#include <awsmock/repository/transfer/TransferMongoRepository.h>

namespace Awsmock::Database {

    enum class BackendType {
        MONGODB,
        MEMORY
    };

    /**
     * @brief Repository factory
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class RepositoryFactory {

      public:

        static RepositoryFactory &instance() {
            static RepositoryFactory instance;
            return instance;
        }

        void initialize(const BackendType type) {
            _backend = type;
            _moduleRepo = createModuleRepository();
            _snsRepo = createSNSRepository();
            _sqsRepo = createSQSRepository();
            _cognitoRepo = createCognitoRepository();
            _monitoringRepo = createMonitoringRepository();
            _kmsRepo = createKMSRepository();
            _transferRepo = createTransferRepository();
            _ssmRepo = createSSMRepository();
            _dynamodbRepo = createDynamodbRepository();
        }

        [[nodiscard]]
        std::shared_ptr<IModuleRepository> moduleRepository() const { return _moduleRepo; }

        [[nodiscard]]
        std::shared_ptr<ISNSRepository> snsRepository() const { return _snsRepo; }

        [[nodiscard]]
        std::shared_ptr<ISQSRepository> sqsRepository() const { return _sqsRepo; }

        [[nodiscard]]
        std::shared_ptr<ICognitoRepository> cognitoRepository() const { return _cognitoRepo; }

        [[nodiscard]]
        std::shared_ptr<IMonitoringRepository> monitoringRepository() const { return _monitoringRepo; }

        [[nodiscard]]
        std::shared_ptr<IKMSRepository> kmsRepository() const { return _kmsRepo; }

        [[nodiscard]]
        std::shared_ptr<ITransferRepository> transferRepository() const { return _transferRepo; }

        [[nodiscard]]
        std::shared_ptr<ISSMRepository> ssmRepository() const { return _ssmRepo; }

        [[nodiscard]]
        std::shared_ptr<IDynamoDbRepository> dynamodbRepository() const { return _dynamodbRepo; }

      private:

        BackendType _backend = BackendType::MONGODB;
        std::shared_ptr<IModuleRepository> _moduleRepo;
        std::shared_ptr<ISNSRepository> _snsRepo;
        std::shared_ptr<ISQSRepository> _sqsRepo;
        std::shared_ptr<ICognitoRepository> _cognitoRepo;
        std::shared_ptr<IMonitoringRepository> _monitoringRepo;
        std::shared_ptr<IKMSRepository> _kmsRepo;
        std::shared_ptr<ITransferRepository> _transferRepo;
        std::shared_ptr<ISSMRepository> _ssmRepo;
        std::shared_ptr<IDynamoDbRepository> _dynamodbRepo;

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

        [[nodiscard]]
        std::shared_ptr<IMonitoringRepository> createMonitoringRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<MonitoringMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<MonitoringMemoryRepository>();
            }
            return std::make_shared<MonitoringMemoryRepository>();
        }

        [[nodiscard]]
        std::shared_ptr<IKMSRepository> createKMSRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<KMSMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<KMSMemoryRepository>();
            }
            return std::make_shared<KMSMemoryRepository>();
        }

        [[nodiscard]]
        std::shared_ptr<ISSMRepository> createSSMRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<SSMMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<SSMMemoryRepository>();
            }
            return std::make_shared<SSMMemoryRepository>();
        }

        [[nodiscard]]
        std::shared_ptr<ITransferRepository> createTransferRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<TransferMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<TransferMemoryRepository>();
            }
            return std::make_shared<TransferMemoryRepository>();
        }

        [[nodiscard]]
        std::shared_ptr<IDynamoDbRepository> createDynamodbRepository() const {
            switch (_backend) {
                case BackendType::MONGODB:
                    return std::make_shared<DynamoDbMongoRepository>();
                case BackendType::MEMORY:
                    return std::make_shared<DynamoDbMemoryRepository>();
            }
            return std::make_shared<DynamoDbMemoryRepository>();
        }
    };

}// namespace Awsmock::Database
