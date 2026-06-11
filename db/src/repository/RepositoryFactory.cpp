// Created by vogje01 on 5/24/26.

#include <awsmock/repository/RepositoryFactory.h>

namespace Awsmock::Database {

    const std::map<std::string, IndexDefinition> RepositoryFactory::indexDefinitions = {
            // SQS messages
            {"sqs_message_idx1", {"sqs_message", {{"queueUrl", 1}, {"status", 1}, {"reset", 1}}, false}},
            {"sqs_message_idx2", {"sqs_message", {{"queueUrl", 1}, {"status", 1}, {"retries", 1}}, false}},
            {"sqs_message_idx3", {"sqs_message", {{"receiptHandle", 1}}, false}},
            {"sqs_message_idx4", {"sqs_message", {{"queueArn", 1}, {"status", 1}, {"size", 1}}, false}},
            {"sqs_message_idx5", {"sqs_message", {{"queueArn", 1}, {"created", 1}}, false}},
            // SQS queues
            {"sqs_queue_idx1", {"sqs_queue", {{"region", 1}, {"name", 1}}, true}},
            // SNS messages
            {"sns_message_idx1", {"sns_message", {{"region", 1}, {"topicArn", 1}}, false}},
            // SNS topics
            {"sns_topic_idx1", {"sns_topic", {{"region", 1}, {"topicName", 1}}, false}},
            {"sns_topic_idx2", {"sns_topic", {{"topicArn", 1}}, true}},
            // S3 buckets
            {"s3_bucket_idx1", {"s3_bucket", {{"region", 1}, {"name", 1}}, false}},
            {"s3_bucket_idx2", {"s3_bucket", {{"arn", 1}}, true}},
            // S3 objects
            {"s3_object_idx1", {"s3_object", {{"region", 1}, {"bucket", 1}, {"key", 1}}, false}},
            {"s3_object_idx2", {"s3_object", {{"internalName", 1}}, false}},
            // Modules
            {"module_idx1", {"module", {{"name", 1}, {"state", 1}}, false}},
            {"module_idx2", {"module", {{"name", 1}}, true}},
            // KMS
            {"kms_idx1", {"kms", {{"region", 1}, {"keyId", 1}}, false}},
            // Applications
            {"application_idx1", {"application", {{"region", 1}, {"name", 1}}, false}},
            {"application_idx2", {"application", {{"region", 1}, {"name", 1}, {"status", 1}}, false}},
            // Monitoring
            {"monitoring_idx1", {"monitoring", {{"name", 1}, {"created", 1}}, false}},
            {"monitoring_idx2", {"monitoring", {{"name", 1}, {"labelName", 1}, {"labelValue", 1}, {"created", 1}}, false}},
            {"monitoring_idx3", {"monitoring", {{"name", 1}, {"labelName", 1}, {"labelValue", 1}}, false}},
            // DynamoDb tables
            {"dynamodb_table_idx1", {"dynamodb_table", {{"region", 1}, {"name", 1}}, true}},
            {"dynamodb_table_idx2", {"dynamodb_table", {{"region", 1}, {"name", 1}, {"itemCount", -1}}, false}},
            // DynamoDb items
            {"dynamodb_item_idx1", {"dynamodb_item", {{"tableName", 1}}, false}},
            {"dynamodb_item_idx2", {"dynamodb_item", {{"tableName", 1}, {"partitionKey", 1}}, false}},
            {"dynamodb_item_idx3", {"dynamodb_item", {{"tableName", 1}, {"partitionKey", 1}, {"sortKey", 1}}, false}},
    };

    void RepositoryFactory::initialize(const BackendType &type, const std::string &databaseName) {

        // Save database name/type
        _backend = type;
        _databaseName = databaseName;

        // Initialize pool
        ConnectionPool::instance().Configure();

        // Repositeries
        _moduleRepo = createModuleRepository();
        _snsRepo = createSNSRepository();
        _sqsRepo = createSQSRepository();
        _s3Repo = createS3Repository();
        _cognitoRepo = createCognitoRepository();
        _monitoringRepo = createMonitoringRepository();
        _kmsRepo = createKMSRepository();
        _transferRepo = createTransferRepository();
        _ssmRepo = createSSMRepository();
        _dynamodbRepo = createDynamodbRepository();
        _lambdaRepo = createLambdaRepository();
        _applicationRepo = createApplicationRepository();
        _secretsmanagerRepo = createSecretsManagerRepository();
        _apigatewayRepo = createApiGatewayRepository();
    }

    void RepositoryFactory::createIndexes() const {
        if (_backend == BackendType::MONGODB) {

            const auto entry = ConnectionPool::instance().GetConnection();
            const mongocxx::database database = (*entry)[_databaseName];

            log_info << "Start creating indexes";

            for (const auto &indexName: std::views::keys(indexDefinitions)) {
                createIndex(database, indexName);
            }
            log_info << "Finished creating indexes, count: " << indexDefinitions.size();
        }
    }

    void RepositoryFactory::createIndex(const mongocxx::database &database, const std::string &indexName) const {
        log_trace << "Start creating index, name: " << indexName;
        auto [collectionName, indexColumns, unique] = indexDefinitions.at(indexName);

        document queryDoc;
        for (const auto &[columns, direction]: indexColumns) {
            queryDoc.append(kvp(columns, direction));
        }

        mongocxx::options::index options{};
        options.name(indexName);
        if (unique) {
            options.unique(true);
        }
        database[collectionName].create_index(queryDoc.extract(), options);
        log_info << "Database index created, name: " << indexName;
    }

    std::shared_ptr<IModuleRepository> RepositoryFactory::createModuleRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<ModuleMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<ModuleMemoryRepository>();
        }
        return std::make_shared<ModuleMemoryRepository>();
    }

    std::shared_ptr<ISNSRepository> RepositoryFactory::createSNSRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<SNSMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<SNSMemoryRepository>();
        }
        return std::make_shared<SNSMemoryRepository>();
    }

    std::shared_ptr<ISQSRepository> RepositoryFactory::createSQSRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<SQSMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<SQSMemoryRepository>();
        }
        return std::make_shared<SQSMemoryRepository>();
    }

    std::shared_ptr<IS3Repository> RepositoryFactory::createS3Repository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<S3MongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<S3MemoryRepository>();
        }
        return std::make_shared<S3MemoryRepository>();
    }

    std::shared_ptr<ICognitoRepository> RepositoryFactory::createCognitoRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<CognitoMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<CognitoMemoryRepository>();
        }
        return std::make_shared<CognitoMemoryRepository>();
    }

    std::shared_ptr<IMonitoringRepository> RepositoryFactory::createMonitoringRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<MonitoringMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<MonitoringMemoryRepository>();
        }
        return std::make_shared<MonitoringMemoryRepository>();
    }

    std::shared_ptr<IKMSRepository> RepositoryFactory::createKMSRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<KMSMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<KMSMemoryRepository>();
        }
        return std::make_shared<KMSMemoryRepository>();
    }

    std::shared_ptr<ISSMRepository> RepositoryFactory::createSSMRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<SSMMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<SSMMemoryRepository>();
        }
        return std::make_shared<SSMMemoryRepository>();
    }

    std::shared_ptr<ITransferRepository> RepositoryFactory::createTransferRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<TransferMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<TransferMemoryRepository>();
        }
        return std::make_shared<TransferMemoryRepository>();
    }

    std::shared_ptr<IDynamoDbRepository> RepositoryFactory::createDynamodbRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<DynamoDbMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<DynamoDbMemoryRepository>();
        }
        return std::make_shared<DynamoDbMemoryRepository>();
    }

    std::shared_ptr<ILambdaRepository> RepositoryFactory::createLambdaRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<LambdaMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<LambdaMemoryRepository>();
        }
        return std::make_shared<LambdaMemoryRepository>();
    }

    std::shared_ptr<IApplicationRepository> RepositoryFactory::createApplicationRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<ApplicationMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<ApplicationMemoryRepository>();
        }
        return std::make_shared<ApplicationMemoryRepository>();
    }

    std::shared_ptr<ISecretsManagerRepository> RepositoryFactory::createSecretsManagerRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<SecretsManagerMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<SecretsManagerMemoryRepository>();
        }
        return std::make_shared<SecretsManagerMemoryRepository>();
    }

    std::shared_ptr<IApiGatewayRepository> RepositoryFactory::createApiGatewayRepository() const {
        switch (_backend) {
            case BackendType::MONGODB:
                return std::make_shared<ApiGatewayMongoRepository>();
            case BackendType::MEMORY:
                return std::make_shared<ApiGatewayMemoryRepository>();
        }
        return std::make_shared<ApiGatewayMemoryRepository>();
    }

}// namespace Awsmock::Database
