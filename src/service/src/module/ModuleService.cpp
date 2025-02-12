//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/service/module/ModuleService.h>

namespace AwsMock::Service {

    Database::Entity::Module::ModuleList ModuleService::ListModules() const {

        Database::Entity::Module::ModuleList modules = _moduleDatabase.ListModules();
        log_debug << "Module list, count: " << modules.size();
        return modules;
    }

    Dto::Module::ListModuleNamesResponse ModuleService::ListModuleNames() const {

        Database::Entity::Module::ModuleList modules = _moduleDatabase.ListModules();
        log_debug << "Module list, count: " << modules.size();
        Dto::Module::ListModuleNamesResponse moduleNamesResponse;
        for (const auto &module: modules) {
            moduleNamesResponse.moduleNames.emplace_back(module.name);
        }
        return moduleNamesResponse;
    }

    Dto::Module::Module::ModuleList ModuleService::StartModules(Dto::Module::Module::ModuleList &modules) const {

        ModuleMap moduleMap = ModuleMap::instance();
        for (auto const &m: modules) {

            // Set state
            if (Database::Entity::Module::Module module = _moduleDatabase.GetModuleByName(m.name); module.state != Database::Entity::Module::ModuleState::RUNNING) {

                // Set state
                module = _moduleDatabase.SetState(m.name, Database::Entity::Module::ModuleState::RUNNING);

                if (module.name == "database") {
                    _moduleDatabase.StartDatabase();
                } else {
                    //moduleMap.GetModule(module.name)->Start();
                }
                log_info << "Module " << m.name << " started";
            }
        }
        return Dto::Module::Mapper::map(_moduleDatabase.ListModules());
    }

    Dto::Module::Module::ModuleList ModuleService::StopModules(Dto::Module::Module::ModuleList &modules) const {

        for (auto const &m: modules) {

            // Set state
            if (Database::Entity::Module::Module module = _moduleDatabase.GetModuleByName(m.name); module.state == Database::Entity::Module::ModuleState::RUNNING) {

                // Set state
                module = _moduleDatabase.SetState(m.name, Database::Entity::Module::ModuleState::STOPPED);
                log_info << "Module " << module.name << " topped";

                // Stop module
                if (m.name == "database") {
                    _moduleDatabase.StopDatabase();
                } else {
                    for (const auto &[fst, snd]: ModuleMap::instance().GetModuleMap()) {
                        if (m.name == fst) {
                            //server.second->Stop();
                        }
                    }
                }
            }
            log_info << "Module " + m.name + " stopped";
        }

        return Dto::Module::Mapper::map(_moduleDatabase.ListModules());
    }

    Dto::Module::ExportInfrastructureResponse ModuleService::ExportInfrastructure(const Dto::Module::ExportInfrastructureRequest &request) {

        Dto::Module::Infrastructure infrastructure = {};

        for (const auto &module: request.modules) {

            if (module == "s3") {

                Database::S3Database &_s3Database = Database::S3Database::instance();
                infrastructure.s3Buckets = _s3Database.ExportBuckets({{.column = "name", .sortDirection = 1}});
                if (request.includeObjects) {
                    infrastructure.s3Objects = _s3Database.ListObjects();
                }

            } else if (module == "sqs") {

                Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
                infrastructure.sqsQueues = _sqsDatabase.ExportQueues({{.column = "name", .sortDirection = 1}});
                if (request.includeObjects) {
                    infrastructure.sqsMessages = _sqsDatabase.ListMessages();
                }

            } else if (module == "sns") {

                Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();
                infrastructure.snsTopics = _snsDatabase.ExportTopics({{.column = "name", .sortDirection = 1}});
                if (request.includeObjects) {
                    infrastructure.snsMessages = _snsDatabase.ListMessages();
                }

            } else if (module == "lambda") {

                Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();
                infrastructure.lambdas = _lambdaDatabase.ExportLambdas({{.column = "name", .sortDirection = 1}});

            } else if (module == "cognito") {

                Database::CognitoDatabase &_cognitoDatabase = Database::CognitoDatabase::instance();
                infrastructure.cognitoUserPools = _cognitoDatabase.ExportUserPools({{.column = "name", .sortDirection = 1}});
                infrastructure.cognitoUserGroups = _cognitoDatabase.ExportGroups({{.column = "name", .sortDirection = 1}});
                infrastructure.cognitoUsers = _cognitoDatabase.ExportUsers({{.column = "name", .sortDirection = 1}});

            } else if (module == "dynamodb") {

                Database::DynamoDbDatabase &_dynamoDbDatabase = Database::DynamoDbDatabase::instance();
                infrastructure.dynamoDbTables = _dynamoDbDatabase.ListTables();
                if (request.includeObjects) {
                    infrastructure.dynamoDbItems = _dynamoDbDatabase.ListItems();
                }

            } else if (module == "secretsmanager") {

                Database::SecretsManagerDatabase &_secretsManagerDatabase = Database::SecretsManagerDatabase::instance();
                infrastructure.secrets = _secretsManagerDatabase.ListSecrets();

            } else if (module == "transfer") {

                Database::TransferDatabase &_transferDatabase = Database::TransferDatabase::instance();
                infrastructure.transferServers = _transferDatabase.ListServers();

            } else if (module == "kms") {

                Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
                infrastructure.kmsKeys = _kmsDatabase.ListKeys();

            } else if (module == "ssm") {

                Database::SSMDatabase &_ssmDatabase = Database::SSMDatabase::instance();
                infrastructure.ssmParameters = _ssmDatabase.ListParameters();
            }
        }
        return {.infrastructure = infrastructure, .includeObjects = request.includeObjects, .prettyPrint = request.prettyPrint};
    }

    void ModuleService::ImportInfrastructure(const std::string &jsonString) {
        log_info << "Importing services, length: " << jsonString.length();

        Dto::Module::Infrastructure infrastructure;
        infrastructure.FromJson(jsonString);

        // S3
        if (!infrastructure.s3Buckets.empty() || !infrastructure.s3Objects.empty()) {
            const auto _s3Database = std::make_shared<Database::S3Database>();
            if (!infrastructure.s3Buckets.empty()) {
                for (auto &bucket: infrastructure.s3Buckets) {
                    bucket.modified = system_clock::now();
                    _s3Database->CreateOrUpdateBucket(bucket);
                }
                log_info << "S3 buckets imported, count: " << infrastructure.s3Buckets.size();
            }
            if (!infrastructure.s3Objects.empty()) {
                for (auto &object: infrastructure.s3Objects) {
                    object.modified = system_clock::now();
                    _s3Database->CreateOrUpdateObject(object);
                }
                log_info << "S3 objects imported, count: " << infrastructure.s3Objects.size();
            }
        }

        // SQS
        if (!infrastructure.sqsQueues.empty() || !infrastructure.sqsMessages.empty()) {
            const Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
            if (!infrastructure.sqsQueues.empty()) {
                for (auto &queue: infrastructure.sqsQueues) {
                    queue.modified = system_clock::now();
                    _sqsDatabase.CreateOrUpdateQueue(queue);
                    log_debug << "SQS queues imported, url: " << queue.queueUrl;
                }
                log_info << "SQS queues imported, count: " << infrastructure.sqsQueues.size();
            }
            if (!infrastructure.sqsMessages.empty()) {
                for (auto &message: infrastructure.sqsMessages) {
                    message.modified = system_clock::now();
                    _sqsDatabase.CreateOrUpdateMessage(message);
                    log_debug << "SQS queues imported, queueArn: " << message.queueArn;
                }
                log_info << "SQS resources imported, count: " << infrastructure.sqsMessages.size();
            }
        }

        // SNS
        if (!infrastructure.snsTopics.empty() || !infrastructure.snsMessages.empty()) {
            const Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();
            if (!infrastructure.snsTopics.empty()) {
                for (auto &topic: infrastructure.snsTopics) {
                    topic.modified = system_clock::now();
                    _snsDatabase.CreateOrUpdateTopic(topic);
                }
                log_info << "SNS topics imported, count: " << infrastructure.snsTopics.size();
            }
            if (!infrastructure.snsMessages.empty()) {
                for (auto &message: infrastructure.snsMessages) {
                    message.modified = system_clock::now();
                    _snsDatabase.CreateOrUpdateMessage(message);
                }
                log_info << "SNS resources imported, count: " << infrastructure.snsMessages.size();
            }
        }

        // Lambdas
        if (!infrastructure.lambdas.empty()) {
            Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();
            for (auto &lambda: infrastructure.lambdas) {
                _lambdaDatabase.ImportLambda(lambda);
            }
            log_info << "Lambda functions imported, count: " << infrastructure.lambdas.size();
        }

        // Transfer server
        if (!infrastructure.transferServers.empty()) {
            const Database::TransferDatabase &_transferDatabase = Database::TransferDatabase::instance();
            for (auto &transfer: infrastructure.transferServers) {
                transfer.modified = system_clock::now();
                transfer = _transferDatabase.CreateOrUpdateTransfer(transfer);
            }
            log_info << "Transfer servers imported, count: " << infrastructure.transferServers.size();
        }

        // Cognito
        if (!infrastructure.cognitoUserPools.empty() || !infrastructure.cognitoUsers.empty()) {
            Database::CognitoDatabase &_cognitoDatabase = Database::CognitoDatabase::instance();
            if (!infrastructure.cognitoUserPools.empty()) {
                for (auto &userPool: infrastructure.cognitoUserPools) {
                    userPool.modified = system_clock::now();
                    _cognitoDatabase.CreateOrUpdateUserPool(userPool);
                }
                log_info << "Cognito user pools imported, count: " << infrastructure.cognitoUserPools.size();
            }
            if (!infrastructure.cognitoUsers.empty()) {
                for (auto &user: infrastructure.cognitoUsers) {
                    user.modified = system_clock::now();
                    _cognitoDatabase.CreateOrUpdateUser(user);
                }
                log_info << "Cognito users imported, count: " << infrastructure.cognitoUsers.size();
            }
        }

        // DynamoDB
        if (!infrastructure.dynamoDbTables.empty() || !infrastructure.dynamoDbItems.empty()) {
            DynamoDbService _dynamoDbService;
            const Database::DynamoDbDatabase &_dynamoDatabase = Database::DynamoDbDatabase::instance();
            if (!infrastructure.dynamoDbTables.empty()) {
                for (auto &table: infrastructure.dynamoDbTables) {
                    if (!_dynamoDbService.ExistTable(table.region, table.name)) {
                        constexpr Dto::DynamoDb::ProvisionedThroughput provisionedThroughput = {.readCapacityUnits = 1, .writeCapacityUnits = 1};
                        Dto::DynamoDb::CreateTableRequest request = {.region = table.region, .tableName = table.name, .attributes = table.attributes, .keySchemas = table.keySchemas, .provisionedThroughput = provisionedThroughput, .tags = table.tags};
                        request.body = request.ToJson();
                        Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);
                    } else {
                        _dynamoDatabase.CreateOrUpdateTable(table);
                    }
                    log_debug << "DynamoDB table created: " << table.name;
                }
                log_info << "DynamoDb tables imported, count: " << infrastructure.dynamoDbTables.size();
            }
            if (!infrastructure.dynamoDbItems.empty()) {
                for (auto &item: infrastructure.dynamoDbItems) {
                    if (_dynamoDbService.ExistTable(item.region, item.tableName)) {
                        Dto::DynamoDb::PutItemRequest request = {.region = item.region, .tableName = item.tableName, .attributes = Dto::DynamoDb::Mapper::map(item.attributes), .keys = Dto::DynamoDb::Mapper::map(item.keys)};
                        request.body = request.ToJson();
                        Dto::DynamoDb::PutItemResponse response = _dynamoDbService.PutItem(request);
                        log_debug << "DynamoDB item created, tableName: " << response.tableName;
                    }
                }
                log_info << "DynamoDb items imported, count: " << infrastructure.dynamoDbItems.size();
            }
        }

        // SecretsManager
        if (!infrastructure.secrets.empty()) {
            const Database::SecretsManagerDatabase &_secretsDatabase = Database::SecretsManagerDatabase::instance();
            for (auto &secret: infrastructure.secrets) {
                secret = _secretsDatabase.CreateOrUpdateSecret(secret);
            }
            log_info << "Secrets imported, count: " << infrastructure.secrets.size();
        }

        // KMS
        if (!infrastructure.kmsKeys.empty()) {
            Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
            for (auto &key: infrastructure.kmsKeys) {
                _kmsDatabase.UpsertKey(key);
            }
            log_info << "Secrets imported, count: " << infrastructure.secrets.size();
        }

        // SSM
        if (!infrastructure.ssmParameters.empty()) {
            const Database::SSMDatabase &_ssmDatabase = Database::SSMDatabase::instance();
            for (auto &parameter: infrastructure.ssmParameters) {
                _ssmDatabase.UpsertParameter(parameter);
            }
            log_info << "Secrets imported, count: " << infrastructure.secrets.size();
        }
    }

    void ModuleService::CleanInfrastructure(const Dto::Module::CleanInfrastructureRequest &request) {
        log_info << "Cleaning services, length: " << request.modules.size();

        CleanObjects(request);

        // Clean only objects
        if (request.onlyObjects) {
            return;
        }

        long count = 0;
        for (const auto &m: request.modules) {

            if (m == "s3") {
                count += Database::S3Database::instance().DeleteAllBuckets();
            } else if (m == "sqs") {
                count += Database::SQSDatabase::instance().DeleteAllQueues();
            } else if (m == "sns") {
                count += Database::SNSDatabase::instance().DeleteAllTopics();
            } else if (m == "dynamodb") {
                DynamoDbService _dynamoDbService;
                count += _dynamoDbService.DeleteAllTables();
            } else if (m == "transfer") {
                count += Database::TransferDatabase::instance().DeleteAllTransfers();
            }
        }
    }

    void ModuleService::CleanObjects(const Dto::Module::CleanInfrastructureRequest &request) {
        log_info << "Cleaning objects, length: " << request.modules.size();

        for (const auto &m: request.modules) {
            if (m == "s3") {
                Database::S3Database::instance().DeleteAllObjects();
            } else if (m == "sqs") {
                Database::SQSDatabase::instance().DeleteAllMessages();
            } else if (m == "sns") {
                Database::SNSDatabase::instance().DeleteAllMessages();
            } else if (m == "lambda") {
                Database::LambdaDatabase::instance().DeleteAllLambdas();
            } else if (m == "cognito") {
                Database::CognitoDatabase::instance().DeleteAllUsers();
                Database::CognitoDatabase::instance().DeleteAllUserPools();
                Database::CognitoDatabase::instance().DeleteAllGroups();
            } else if (m == "dynamodb") {
                Database::DynamoDbDatabase::instance().DeleteAllItems();
            } else if (m == "secretsmanager") {
                Database::SecretsManagerDatabase::instance().DeleteAllSecrets();
            } else if (m == "kms") {
                Database::KMSDatabase::instance().DeleteAllKeys();
            } else if (m == "ssm") {
                Database::SSMDatabase::instance().DeleteAllParameters();
            } else if (m == "transfer") {
                Database::S3Database::instance().DeleteObjects("transfer-server");
            }
        }
    }
}// namespace AwsMock::Service
