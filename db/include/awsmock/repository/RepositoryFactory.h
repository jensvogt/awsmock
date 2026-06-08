//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <memory>

// Awsmock includes
#include <awsmock/repository/apigateway/ApiGatewayMemoryRepository.h>
#include <awsmock/repository/apigateway/ApiGatewayMongoRepository.h>
#include <awsmock/repository/apigateway/IApiGatewayRepository.h>
#include <awsmock/repository/application/ApplicationMemoryRepository.h>
#include <awsmock/repository/application/ApplicationMongoRepository.h>
#include <awsmock/repository/application/IApplicationRepository.h>
#include <awsmock/repository/cognito/CognitoMemoryRepository.h>
#include <awsmock/repository/cognito/CognitoMongoRepository.h>
#include <awsmock/repository/cognito/ICognitoRepository.h>
#include <awsmock/repository/dynamodb/DynamoDbMemoryRepository.h>
#include <awsmock/repository/dynamodb/DynamoDbMongoRepository.h>
#include <awsmock/repository/dynamodb/IDynamoDbRepository.h>
#include <awsmock/repository/kms/IKMSRepository.h>
#include <awsmock/repository/kms/KMSMemoryRepository.h>
#include <awsmock/repository/kms/KMSMongoRepository.h>
#include <awsmock/repository/lambda/ILambdaRepository.h>
#include <awsmock/repository/lambda/LambdaMemoryRepository.h>
#include <awsmock/repository/lambda/LambdaMongoRepository.h>
#include <awsmock/repository/module/IModuleRepository.h>
#include <awsmock/repository/module/ModuleMemoryRepository.h>
#include <awsmock/repository/module/ModuleMongoRepository.h>
#include <awsmock/repository/monitoring/IMonitoringRepository.h>
#include <awsmock/repository/monitoring/MonitoringMemoryRepository.h>
#include <awsmock/repository/monitoring/MonitoringMongoRepository.h>
#include <awsmock/repository/s3/IS3Repository.h>
#include <awsmock/repository/s3/S3MemoryRepository.h>
#include <awsmock/repository/s3/S3MongoRepository.h>
#include <awsmock/repository/secretsmanager/ISecretsManagerRepository.h>
#include <awsmock/repository/secretsmanager/SecretsManagerMemoryRepository.h>
#include <awsmock/repository/secretsmanager/SecretsManagerMongoRepository.h>
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

    struct IndexColumnDefinition {

        /**
         * Column name
         */
        std::string columns;

        /**
         * Index direction
         */
        int direction;
    };

    struct IndexDefinition {

        /**
         * Collection name
         */
        std::string collectionName;

        /**
         * Columns definitions
         */
        std::vector<IndexColumnDefinition> indexColumns;

        /**
         * Unique index
         */
        bool unique = false;
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

        void initialize(const BackendType &type, const std::string &databaseName);

        void createIndexes() const;

        [[nodiscard]]
        std::shared_ptr<IModuleRepository> moduleRepository() const { return _moduleRepo; }

        [[nodiscard]]
        std::shared_ptr<ISNSRepository> snsRepository() const { return _snsRepo; }

        [[nodiscard]]
        std::shared_ptr<ISQSRepository> sqsRepository() const { return _sqsRepo; }

        [[nodiscard]]
        std::shared_ptr<IS3Repository> s3Repository() const { return _s3Repo; }

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

        [[nodiscard]]
        std::shared_ptr<ILambdaRepository> lambdaRepository() const { return _lambdaRepo; }

        [[nodiscard]]
        std::shared_ptr<IApplicationRepository> applicationRepository() const { return _applicationRepo; }

        [[nodiscard]]
        std::shared_ptr<ISecretsManagerRepository> secretsmanagerRepository() const { return _secretsmanagerRepo; }

        [[nodiscard]]
        std::shared_ptr<IApiGatewayRepository> apigatewayRepository() const { return _apigatewayRepo; }

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Database"};

        /**
         * @brief Database type
         */
        BackendType _backend = BackendType::MONGODB;

        /**
         * @brief Database type
         */
        std::string _databaseName;

        /**
         * @brief Repos
         */
        std::shared_ptr<IModuleRepository> _moduleRepo;
        std::shared_ptr<ISNSRepository> _snsRepo;
        std::shared_ptr<ISQSRepository> _sqsRepo;
        std::shared_ptr<IS3Repository> _s3Repo;
        std::shared_ptr<ICognitoRepository> _cognitoRepo;
        std::shared_ptr<IMonitoringRepository> _monitoringRepo;
        std::shared_ptr<IKMSRepository> _kmsRepo;
        std::shared_ptr<ITransferRepository> _transferRepo;
        std::shared_ptr<ISSMRepository> _ssmRepo;
        std::shared_ptr<IDynamoDbRepository> _dynamodbRepo;
        std::shared_ptr<ILambdaRepository> _lambdaRepo;
        std::shared_ptr<IApplicationRepository> _applicationRepo;
        std::shared_ptr<ISecretsManagerRepository> _secretsmanagerRepo;
        std::shared_ptr<IApiGatewayRepository> _apigatewayRepo;

        [[nodiscard]]
        std::shared_ptr<IModuleRepository> createModuleRepository() const;

        [[nodiscard]]
        std::shared_ptr<ISNSRepository> createSNSRepository() const;

        [[nodiscard]]
        std::shared_ptr<ISQSRepository> createSQSRepository() const;

        [[nodiscard]]
        std::shared_ptr<IS3Repository> createS3Repository() const;

        [[nodiscard]]
        std::shared_ptr<ICognitoRepository> createCognitoRepository() const;

        [[nodiscard]]
        std::shared_ptr<IMonitoringRepository> createMonitoringRepository() const;

        [[nodiscard]]
        std::shared_ptr<IKMSRepository> createKMSRepository() const;

        [[nodiscard]]
        std::shared_ptr<ISSMRepository> createSSMRepository() const;

        [[nodiscard]]
        std::shared_ptr<ITransferRepository> createTransferRepository() const;

        [[nodiscard]]
        std::shared_ptr<IDynamoDbRepository> createDynamodbRepository() const;

        [[nodiscard]]
        std::shared_ptr<ILambdaRepository> createLambdaRepository() const;

        [[nodiscard]]
        std::shared_ptr<IApplicationRepository> createApplicationRepository() const;

        [[nodiscard]]
        std::shared_ptr<ISecretsManagerRepository> createSecretsManagerRepository() const;

        [[nodiscard]]
        std::shared_ptr<IApiGatewayRepository> createApiGatewayRepository() const;

        /**
         * @brief Create a single index.
         *
         * @param database database name
         * @param indexName name of  the index
         */
        void createIndex(const mongocxx::database &database, const std::string &indexName) const;

        /**
         * @brief Index definitions
         */
        const static std::map<std::string, IndexDefinition> indexDefinitions;
    };

} // namespace Awsmock::Database
