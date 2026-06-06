//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/service/module/ModuleService.h>

#include "awsmock/dto/module/GetLogLevelRequest.h"

namespace Awsmock::Service {

    std::vector<Database::Entity::Module::Module> ModuleService::ListModules() const {

        std::vector<Database::Entity::Module::Module> modules = _moduleDatabase->listModules();
        log_debug << "Module list, count: " << modules.size();
        return modules;
    }

    Dto::Module::ListModuleNamesResponse ModuleService::ListModuleNames() const {

        const std::vector<Database::Entity::Module::Module> modules = _moduleDatabase->listModules();
        log_debug << "Module list, count: " << modules.size();
        Dto::Module::ListModuleNamesResponse moduleNamesResponse;
        for (const auto &module: modules) {
            moduleNamesResponse.moduleNames.emplace_back(module.name);
        }
        return moduleNamesResponse;
    }

    Dto::Module::Module::ModuleList ModuleService::StartModules(const Dto::Module::Module::ModuleList &modules) const {

        ModuleMap moduleMap = ModuleMap::instance();
        for (auto const &m: modules) {

            // Set state
            if (Database::Entity::Module::Module module = _moduleDatabase->getModuleByName(m.name); module.state != Database::Entity::Module::ModuleState::RUNNING) {

                // Set state
                _moduleDatabase->setState(m.name, Database::Entity::Module::ModuleState::RUNNING);
                log_info << "Module " << module.name << " started";
            }
        }

        // Return updated list
        return Dto::Module::Mapper::map(_moduleDatabase->listModules());
    }

    Dto::Module::Module::ModuleList ModuleService::StopModules(const Dto::Module::Module::ModuleList &modules) const {

        for (auto const &m: modules) {

            // Set state
            if (Database::Entity::Module::Module module = _moduleDatabase->getModuleByName(m.name); module.state == Database::Entity::Module::ModuleState::RUNNING) {

                // Send shutdown call
                ModuleMap::instance().GetModuleMap()[m.name]->Shutdown();

                // Set state
                _moduleDatabase->setState(m.name, Database::Entity::Module::ModuleState::STOPPED);
                log_info << "Module " << module.name << " stopped";
            }
            log_info << "Module " + m.name + " stopped";
        }

        return Dto::Module::Mapper::map(_moduleDatabase->listModules());
    }

    Dto::Module::ExportInfrastructureResponse ModuleService::ExportInfrastructure(const Dto::Module::ExportInfrastructureRequest &request) {

        // Create infrastructure object
        Dto::Module::Infrastructure infrastructure = {};
        Database::SortColumn sortColumn;
        sortColumn.column = "name";
        sortColumn.sortDirection = 1;

        for (const auto &module: request.modules) {

            if (module == "s3") {

                const std::shared_ptr<Database::IS3Repository> _s3Database = Database::RepositoryFactory::instance().s3Repository();
                if (request.IsInfrastructure()) {
                    infrastructure.s3Buckets = _s3Database->exportBuckets({sortColumn});
                }
                if (request.IsObjects()) {
                    infrastructure.s3Objects = _s3Database->listObjects({}, {}, {}, 0, 0, {});
                    infrastructure.s3Objects = _s3Database->listObjects({}, {}, {}, 0, 0, {});
                }

            } else if (module == "sqs") {

                std::shared_ptr<Database::ISQSRepository> _sqsDatabase = Database::RepositoryFactory::instance().sqsRepository();
                if (request.IsInfrastructure()) {
                    infrastructure.sqsQueues = _sqsDatabase->exportQueues({sortColumn});
                }
                if (request.IsObjects()) {
                    infrastructure.sqsMessages = _sqsDatabase->listMessages({});
                }

            } else if (module == "sns") {

                const std::shared_ptr<Database::ISNSRepository> _snsDatabase = Database::RepositoryFactory::instance().snsRepository();
                if (request.IsInfrastructure()) {
                    infrastructure.snsTopics = _snsDatabase->exportTopics({sortColumn});
                }
                if (request.IsObjects()) {
                    infrastructure.snsMessages = _snsDatabase->listMessages({}, {}, 0, 0, {});
                }

            } else if (module == "lambda") {

                const std::shared_ptr<Database::ILambdaRepository> _lambdaDatabase = Database::RepositoryFactory::instance().lambdaRepository();
                if (request.IsInfrastructure()) {
                    infrastructure.lambdas = _lambdaDatabase->exportLambdas({sortColumn});
                }

            } else if (module == "cognito") {

                const std::shared_ptr<Database::ICognitoRepository> _cognitoDatabase = Database::RepositoryFactory::instance().cognitoRepository();
                if (request.IsInfrastructure()) {
                    infrastructure.cognitoUserPools = _cognitoDatabase->exportUserPools({sortColumn});
                    infrastructure.cognitoUserGroups = _cognitoDatabase->exportGroups({sortColumn});
                    infrastructure.cognitoUsers = _cognitoDatabase->exportUsers({sortColumn});
                }

            } else if (module == "dynamodb") {

                std::shared_ptr<Database::IDynamoDbRepository> _dynamoDbDatabase = Database::RepositoryFactory::instance().dynamodbRepository();
                if (request.IsInfrastructure()) {
                    infrastructure.dynamoDbTables = _dynamoDbDatabase->listTables({}, {}, 0, 0, {});
                }
                if (request.IsObjects()) {
                    infrastructure.dynamoDbItems = _dynamoDbDatabase->listItems({}, {}, 0, 0, {});
                }

            } else if (module == "secretsmanager") {

                if (request.IsInfrastructure()) {
                    const std::shared_ptr<Database::ISecretsManagerRepository> _secretsManagerDatabase = Database::RepositoryFactory::instance().secretsmanagerRepository();
                    infrastructure.secrets = _secretsManagerDatabase->ListSecrets();
                }

            } else if (module == "transfer") {

                if (request.IsInfrastructure()) {
                    const std::shared_ptr<Database::ITransferRepository> _transferDatabase = Database::RepositoryFactory::instance().transferRepository();
                    infrastructure.transferServers = _transferDatabase->listServers({}, {}, 0, 0, {});
                }

            } else if (module == "kms") {

                if (request.IsInfrastructure()) {
                    const std::shared_ptr<Database::IKMSRepository> _kmsDatabase = Database::RepositoryFactory::instance().kmsRepository();
                    infrastructure.kmsKeys = _kmsDatabase->listKeys({}, {}, 0, 0, {});
                }

            } else if (module == "ssm") {

                if (request.IsInfrastructure()) {
                    const std::shared_ptr<Database::ISSMRepository> _ssmDatabase = Database::RepositoryFactory::instance().ssmRepository();
                    infrastructure.ssmParameters = _ssmDatabase->listParameters({}, {}, 0, 0, {});
                }

            } else if (module == "application") {

                if (request.IsInfrastructure()) {
                    const std::shared_ptr<Database::IApplicationRepository> _applicationDatabase = Database::RepositoryFactory::instance().applicationRepository();
                    infrastructure.applications = _applicationDatabase->listApplications({}, {}, 0, 0, {});
                }

            } else if (module == "apigateway") {

                if (request.IsInfrastructure()) {
                    const std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();
                    infrastructure.apiKeys = _apiGatewayDatabase->getApiKeys({}, {}, {}, 0);
                }
            }
        }
        Dto::Module::ExportInfrastructureResponse response;
        response.infrastructure = infrastructure;
        response.exportType = request.exportType;
        response.prettyPrint = request.prettyPrint;
        return response;
    }

    void ModuleService::ImportInfrastructure(const Dto::Module::ImportInfrastructureRequest &request) const {
        log_info << "Importing modules, cleanFirst: " << std::boolalpha << request.cleanFirst;

        Dto::Module::Infrastructure infrastructure = request.infrastructure;

        // Check the clean flag
        if (request.cleanFirst) {
            Dto::Module::CleanInfrastructureRequest cleanRequest;
            cleanRequest.modules.emplace_back("All");
            cleanRequest.onlyObjects = request.includeObjects;
            CleanInfrastructure(cleanRequest);
        }

        // S3
        if (!infrastructure.s3Buckets.empty() || !infrastructure.s3Objects.empty()) {
            const std::shared_ptr<Database::IS3Repository> _s3Database = Database::RepositoryFactory::instance().s3Repository();
            if (!infrastructure.s3Buckets.empty()) {
                for (auto &bucket: infrastructure.s3Buckets) {
                    bucket.modified = system_clock::now();
                    bucket = _s3Database->createOrUpdateBucket(bucket);
                }
                log_info << "S3 buckets imported, count: " << infrastructure.s3Buckets.size();
            }
            if (!infrastructure.s3Objects.empty()) {
                for (auto &object: infrastructure.s3Objects) {

                    // Create objects
                    object.modified = system_clock::now();
                    object = _s3Database->createOrUpdateObject(object);

                    // Load local files
                    if (!object.localName.empty()) {
                        ImportLocalS3File(object);
                    }
                }
                log_info << "S3 objects imported, count: " << infrastructure.s3Objects.size();
            }
        }

        // SQS
        if (!infrastructure.sqsQueues.empty() || !infrastructure.sqsMessages.empty()) {
            const std::shared_ptr<Database::ISQSRepository> _sqsDatabase = Database::RepositoryFactory::instance().sqsRepository();
            if (!infrastructure.sqsQueues.empty()) {
                for (auto &queue: infrastructure.sqsQueues) {
                    _sqsDatabase->importQueue(queue);
                    log_debug << "SQS queues imported, name: " << queue.name;
                }
                log_info << "SQS queues imported, count: " << infrastructure.sqsQueues.size();
            }
            if (!infrastructure.sqsMessages.empty()) {
                for (auto &message: infrastructure.sqsMessages) {
                    message.modified = system_clock::now();
                    message = _sqsDatabase->createOrUpdateMessage(message);
                    log_debug << "SQS queues imported, queueArn: " << message.queueArn;
                }
                log_info << "SQS resources imported, count: " << infrastructure.sqsMessages.size();
            }
        }

        // SNS
        if (!infrastructure.snsTopics.empty() || !infrastructure.snsMessages.empty()) {
            const std::shared_ptr<Database::ISNSRepository> _snsDatabase = Database::RepositoryFactory::instance().snsRepository();
            if (!infrastructure.snsTopics.empty()) {
                for (auto &topic: infrastructure.snsTopics) {
                    topic.modified = system_clock::now();
                    topic = _snsDatabase->createOrUpdateTopic(topic);
                }
                log_info << "SNS topics imported, count: " << infrastructure.snsTopics.size();
            }
            if (!infrastructure.snsMessages.empty()) {
                for (auto &message: infrastructure.snsMessages) {
                    message.modified = system_clock::now();
                    message = _snsDatabase->createOrUpdateMessage(message);
                }
                log_info << "SNS resources imported, count: " << infrastructure.snsMessages.size();
            }
        }

        // Lambdas
        if (!infrastructure.lambdas.empty()) {
            const std::shared_ptr<Database::ILambdaRepository> _lambdaDatabase = Database::RepositoryFactory::instance().lambdaRepository();
            for (auto &lambda: infrastructure.lambdas) {
                lambda = _lambdaDatabase->importLambda(lambda);
            }
            log_info << "Lambda functions imported, count: " << infrastructure.lambdas.size();
        }

        // Transfer server
        if (!infrastructure.transferServers.empty()) {
            const std::shared_ptr<Database::ITransferRepository> _transferDatabase = Database::RepositoryFactory::instance().transferRepository();
            for (auto &transfer: infrastructure.transferServers) {
                transfer.modified = system_clock::now();
                transfer = _transferDatabase->createOrUpdateTransfer(transfer);
            }
            log_info << "Transfer servers imported, count: " << infrastructure.transferServers.size();
        }

        // Cognito
        if (!infrastructure.cognitoUserPools.empty() || !infrastructure.cognitoUsers.empty()) {
            const std::shared_ptr<Database::ICognitoRepository> _cognitoDatabase = Database::RepositoryFactory::instance().cognitoRepository();
            if (!infrastructure.cognitoUserPools.empty()) {
                for (auto &userPool: infrastructure.cognitoUserPools) {
                    userPool.modified = system_clock::now();
                    userPool = _cognitoDatabase->createOrUpdateUserPool(userPool);
                }
                log_info << "Cognito user pools imported, count: " << infrastructure.cognitoUserPools.size();
            }
            if (!infrastructure.cognitoUsers.empty()) {
                for (auto &user: infrastructure.cognitoUsers) {
                    user.modified = system_clock::now();
                    user = _cognitoDatabase->createOrUpdateUser(user);
                }
                log_info << "Cognito users imported, count: " << infrastructure.cognitoUsers.size();
            }
        }

        try {
            // DynamoDB
            if (!infrastructure.dynamoDbTables.empty() || !infrastructure.dynamoDbItems.empty()) {
                const std::shared_ptr<Database::IDynamoDbRepository> _dynamoDatabase = Database::RepositoryFactory::instance().dynamodbRepository();
                if (!infrastructure.dynamoDbTables.empty()) {
                    for (auto &table: infrastructure.dynamoDbTables) {
                        table.modified = system_clock::now();
                        table = _dynamoDatabase->createOrUpdateTable(table);
                    }
                    log_info << "DynamoDB table imported, count: " << infrastructure.cognitoUserPools.size();
                }

                if (!infrastructure.dynamoDbItems.empty()) {
                    for (auto &item: infrastructure.dynamoDbItems) {
                        item.modified = system_clock::now();
                        item.size = sizeof(item) + sizeof(long);
                        item = _dynamoDatabase->createOrUpdateItem(item);
                    }
                    log_info << "DynamoDB item imported, count: " << infrastructure.cognitoUserPools.size();
                }
                _dynamoDatabase->adjustItemCounters();
            }

            // Applications
            if (!infrastructure.applications.empty()) {
                const std::shared_ptr<Database::IApplicationRepository> _applicationDatabase = Database::RepositoryFactory::instance().applicationRepository();
                for (auto &application: infrastructure.applications) {
                    application = _applicationDatabase->importApplication(application);
                }
                log_info << "Applications imported, count: " << infrastructure.applications.size();
            }
        } catch (Core::ServiceException &ex) {
            log_error << "Service exception: " << ex.what();
        }

        // SecretsManager
        if (!infrastructure.secrets.empty()) {
            const std::shared_ptr<Database::ISecretsManagerRepository> _secretsDatabase = Database::RepositoryFactory::instance().secretsmanagerRepository();
            for (auto &secret: infrastructure.secrets) {
                secret = _secretsDatabase->CreateOrUpdateSecret(secret);
            }
            log_info << "Secrets imported, count: " << infrastructure.secrets.size();
        }

        // KMS
        if (!infrastructure.kmsKeys.empty()) {
            const std::shared_ptr<Database::IKMSRepository> _kmsDatabase = Database::RepositoryFactory::instance().kmsRepository();
            for (auto &key: infrastructure.kmsKeys) {
                key = _kmsDatabase->upsertKey(key);
            }
            log_info << "KMS keys imported, count: " << infrastructure.secrets.size();
        }

        // SSM
        if (!infrastructure.ssmParameters.empty()) {
            const std::shared_ptr<Database::ISSMRepository> _ssmDatabase = Database::RepositoryFactory::instance().ssmRepository();
            for (auto &parameter: infrastructure.ssmParameters) {
                parameter = _ssmDatabase->importParameter(parameter);
            }
            log_info << "SSM parameters imported, count: " << infrastructure.ssmParameters.size();
        }

        // API gateway keys
        if (!infrastructure.apiKeys.empty()) {
            const std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();
            for (auto &apiKey: infrastructure.apiKeys) {
                _apiGatewayDatabase->importApiKey(apiKey);
            }
            log_info << "Api gateway api keys imported, count: " << infrastructure.apiKeys.size();
        }

        // API gateway REST APIs
        if (!infrastructure.restApis.empty()) {
            const std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();
            for (auto &restApi: infrastructure.restApis) {
                _apiGatewayDatabase->importRestApis(restApi);
            }
            log_info << "Api gateway rest apis imported, count: " << infrastructure.restApis.size();
        }
    }

    void ModuleService::CleanInfrastructure(const Dto::Module::CleanInfrastructureRequest &request) const {
        log_info << "Cleaning services, length: " << request.modules.size();

        CleanObjects(request);

        // Clean only objects
        if (request.onlyObjects) {
            return;
        }

        long count = 0;
        for (const auto &m: request.modules) {

            if (m == "s3") {
                count += Database::RepositoryFactory::instance().s3Repository()->deleteAllBuckets();
            } else if (m == "sqs") {
                count += Database::RepositoryFactory::instance().sqsRepository()->deleteAllQueues();
            } else if (m == "sns") {
                count += Database::RepositoryFactory::instance().snsRepository()->deleteAllTopics();
            } else if (m == "dynamodb") {
                count += Database::RepositoryFactory::instance().dynamodbRepository()->deleteAllTables();
            } else if (m == "transfer") {
                count += Database::RepositoryFactory::instance().transferRepository()->deleteAllTransfers();
            } else if (m == "application") {
                count += Database::RepositoryFactory::instance().applicationRepository()->deleteAllApplications();
            }
        }
    }

    void ModuleService::CleanObjects(const Dto::Module::CleanInfrastructureRequest &request) const {
        log_info << "Cleaning objects, modules: " << request.modules.size();

        long count = 0;
        for (const auto &m: request.modules) {
            if (m == "s3") {
                count += Database::RepositoryFactory::instance().s3Repository()->deleteAllObjects();
            } else if (m == "sqs") {
                count += Database::RepositoryFactory::instance().sqsRepository()->deleteAllMessages();
            } else if (m == "sns") {
                count += Database::RepositoryFactory::instance().snsRepository()->deleteAllMessages();
            } else if (m == "lambda") {
                count += Database::RepositoryFactory::instance().lambdaRepository()->deleteAllLambdas();
            } else if (m == "cognito") {
                count += Database::RepositoryFactory::instance().cognitoRepository()->deleteAllUsers();
                count += Database::RepositoryFactory::instance().cognitoRepository()->deleteAllUserPools();
                count += Database::RepositoryFactory::instance().cognitoRepository()->deleteAllGroups({});
            } else if (m == "dynamodb") {
                count += Database::RepositoryFactory::instance().dynamodbRepository()->deleteAllItems();
            } else if (m == "secretsmanager") {
                count += Database::RepositoryFactory::instance().secretsmanagerRepository()->DeleteAllSecrets();
            } else if (m == "kms") {
                count += Database::RepositoryFactory::instance().kmsRepository()->deleteAllKeys();
            } else if (m == "ssm") {
                count += Database::RepositoryFactory::instance().ssmRepository()->deleteAllParameters();
            } else if (m == "transfer") {
                count += Database::RepositoryFactory::instance().s3Repository()->deleteObjects("eu-central-1", "transfer-server", {});
                count += Database::RepositoryFactory::instance().transferRepository()->deleteAllTransfers();
            } else if (m == "apigateway") {
                count += Database::RepositoryFactory::instance().apigatewayRepository()->deleteAllKeys();
            }
        }
    }

    void ModuleService::ImportLocalS3File(const Database::Entity::S3::Object &object) const {
        if (Core::FileUtils::FileExists(object.localName)) {
            const auto s3DataDir = Core::Configuration::instance().get<std::string>("awsmock.modules.s3.data-dir");
            const std::string localFilePath = Core::FileUtils::appendPath(s3DataDir, object.internalName);
            Core::FileUtils::CopyTo(object.localName, localFilePath);
            log_info << "Local file imported, bucket: " << object.bucket << ", key: " << object.key << ", localName: " << object.localName;
        } else {
            log_warning << "S3 local file does not exist, path: " << object.internalName;
        }
    }

    void ModuleService::BackupModule(const std::string &module, const Dto::Module::ExportType &exportType) const {

        // Backup file name
        std::string backupFilename = Core::BackupUtils::GetBackupFilename(module);
        log_info << "Creating backup of module, name: " << module << ", file: " << backupFilename;

        // Create export request
        Dto::Module::ExportInfrastructureRequest request;
        request.prettyPrint = true;
        request.exportType = exportType;
        request.modules.emplace_back(module);

        // Do the actual export
        Dto::Module::ExportInfrastructureResponse response = ExportInfrastructure(request);

        // Write the backup file
        std::ofstream backupFile(backupFilename);
        backupFile << response.ToJson();
        backupFile.close();

        // Backup retention
        BackupRetention(module);

        // Sleep for a while, otherwise cron will execute at the same time again
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }

    void ModuleService::BackupRetention(const std::string &module) const {

        // Get the file list
        const int retention = Core::Configuration::instance().get<int>("awsmock.modules." + module + ".backup.count");
        const std::vector<std::string> backupList = Core::BackupUtils::GetBackupFiles(module, retention);
        log_info << "Cleanup backup files, module: " << module << ", count: " << backupList.size();
        for (const auto &file: backupList) {
            Core::FileUtils::RemoveFile(file);
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ModuleService::UpdateLambda(const std::string &name) const {
        const Dto::Module::ExportInfrastructureResponse response = ExportInfrastructure();
        const auto filename = Core::Configuration::instance().get<std::string>("awsmock.autoload.file");
        std::ofstream ofs(filename, std::ios::trunc);
        ofs << response.ToJson();
        ofs.close();
        log_info << "Infrastructure exported, file: " << filename;
    }

    void ModuleService::setLogLevel(const Dto::Module::SetLogLevelRequest &request) const {
        if (request.channel.empty()) {
            const long modified = _moduleDatabase->setAllModulesLoglevel(request.level);
            Core::LogStream::SetSeverity(request.level);
            log_info << "Log level set, channel: all, level: " << request.level << ", count: " << modified;
        } else {
            const long modified = _moduleDatabase->setModuleLoglevel(request.channel, request.level);
            Core::LogStream::SetChannelSeverity(request.channel, request.level);
            log_info << "Log level set, channel: " << request.channel << ", level: " << request.level << ", count: " << modified;
        }
    }

    Dto::Module::GetLogLevelResponse ModuleService::getLogLevels(const Dto::Module::GetLogLevelRequest &request) const {

        Dto::Module::GetLogLevelResponse response;
        for (std::vector<Database::Entity::Module::Module> modules = ListModules(); const auto module: modules) {
            if (request.names.empty() || std::ranges::find(request.names, module.name) != request.names.end()) {
                Dto::Module::LogLevel level = {.name = module.name, .level = module.logLevel};
                response.logLevels.emplace_back(level);
            }
        }
        return response;
    }
} // namespace Awsmock::Service
