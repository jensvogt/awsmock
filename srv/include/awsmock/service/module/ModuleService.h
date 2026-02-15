//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_MODULE_SERVICE_H
#define AWSMOCK_SERVICE_MODULE_SERVICE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BackupUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/Services.h>
#include <awsmock/dto/dynamodb/CreateTableRequest.h>
#include <awsmock/dto/module/CleanInfrastructureRequest.h>
#include <awsmock/dto/module/ExportInfrastructureRequest.h>
#include <awsmock/dto/module/ExportInfrastructureResponse.h>
#include <awsmock/dto/module/ImportInfrastructureRequest.h>
#include <awsmock/dto/module/ListModuleNamesResponse.h>
#include <awsmock/dto/module/mapper/Mapper.h>
#include <awsmock/dto/module/model/Infrastructure.h>
#include <awsmock/dto/module/model/Module.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/ApiGatewayDatabase.h>
#include <awsmock/repository/ApplicationDatabase.h>
#include <awsmock/repository/CognitoDatabase.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/ModuleDatabase.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/repository/SSMDatabase.h>
#include <awsmock/repository/SecretsManagerDatabase.h>
#include <awsmock/repository/TransferDatabase.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>
#include <awsmock/service/module/ModuleMap.h>

namespace AwsMock::Service {

    /**
     * @brief The ModuleService controls the different services
     *
     * @par
     * Provides the import/export functionalities. Exports are provided using the BSON structure directly out of the
     * MongoDB. Imports assume a valid BSON document, which is directly imported into the MongoDB.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ModuleService {

      public:

        /**
         * @brief Constructor
         */
        explicit ModuleService() : _moduleDatabase(Database::ModuleDatabase::instance()) {};

        /**
         * @brief Return a list of all modules
         *
         * @return list of all modules
         */
        [[nodiscard]] Database::Entity::Module::ModuleList ListModules() const;

        /**
         * @brief Starts a module
         *
         * @param modules list of modules
         * @return updated module list
         */
        [[nodiscard]] Dto::Module::Module::ModuleList StartModules(const Dto::Module::Module::ModuleList &modules) const;

        /**
         * @brief Stops one or several modules
         *
         * @param modules module list
         * @return updated module list
         */
        Dto::Module::Module::ModuleList StopModules(Dto::Module::Module::ModuleList &modules) const;

        /**
         * @brief List module names
         *
         * @return JSON string
         */
        [[nodiscard]] Dto::Module::ListModuleNamesResponse ListModuleNames() const;

        /**
         * @brief Exports the current infrastructure
         *
         * @par
         * The export file will be in BSON format, as provided by the MongoDB BSON implementation.
         *
         * @param request export infrastructure request
         * @return JSON string
         */
        static Dto::Module::ExportInfrastructureResponse ExportInfrastructure(const Dto::Module::ExportInfrastructureRequest &request = {});

        /**
         * @brief Import the infrastructure
         *
         * @par
         * The import file should be in MongoDB BSON format. Add SQS queue URLs will be adjusted according to the localhost/port coming from the configuration file.
         *
         * @param request infrastructure import request
         */
        static void ImportInfrastructure(const Dto::Module::ImportInfrastructureRequest &request);

        /**
         * @brief Cleans the current infrastructure.
         *
         * @par
         * All SQS queues, SNS topics, S3 buckets, etc. will be deleted, as well as all objects.
         *
         * @param request clean infrastructure request
         */
        static void CleanInfrastructure(const Dto::Module::CleanInfrastructureRequest &request);

        /**
         * @brief Cleans the objects from the infrastructure.
         *
         * @par
         * Cleans all objects from the infrastructure. This means all SQS resources, SNS resources, S3 object keys, etc. will be deleted.
         *
         * @param request clean infrastructure request
         */
        static void CleanObjects(const Dto::Module::CleanInfrastructureRequest &request);

        /**
         * @brief Backup infrastructure
         *
         * @param module module name
         * @param includeObjects include all objects
         */
        static void BackupModule(const std::string &module, bool includeObjects = false);

        /**
         * @brief Cleanup backups, keep only the number of backups which are defined in the module retention property
         *
         * @param module module name
         */
        static void BackupRetention(const std::string &module);

        /**
         * @brief Update lambda slot, signaled whenever a lambda function is updated
         *
         * @param name lambda name
         */
        void UpdateLambda(const std::string &name);

      private:

        /**
         * @brief Import a local S3 file
         *
         * @par
         * The local file name should be a absolute path.
         *
         * @param object S3 object to import
         */
        static void ImportLocalS3File(const Database::Entity::S3::Object &object);

        /**
         * @brief Module database
         */
        Database::ModuleDatabase &_moduleDatabase;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_MODULE_SERVICE_H
