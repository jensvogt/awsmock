//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_MANAGER_H
#define AWSMOCK_MANAGER_H


// AwsMock includes
#include <awsmock/service/cognito/CognitoServer.h>
#include <awsmock/service/dynamodb/DynamoDbServer.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/kms/KMSServer.h>
#include <awsmock/service/lambda/LambdaServer.h>
#include <awsmock/service/monitoring/MonitoringServer.h>
#include <awsmock/service/s3/S3Server.h>
#include <awsmock/service/secretsmanager/SecretsManagerServer.h>
#include <awsmock/service/sns/SNSServer.h>
#include <awsmock/service/sqs/SQSServer.h>
#include <awsmock/service/ssm/SSMServer.h>
#include <awsmock/service/transfer/TransferServer.h>

#define DEFAULT_MONGO_DBNAME "awsmock"
#define DEFAULT_MONGO_DBUSER "root"
#define DEFAULT_MONGO_DBPWD "password"
#define DEFAULT_MONGO_DBHOST "localhost"
#define DEFAULT_MONGO_DBPORT 27017
#define DEFAULT_MONGO_POOL_SIZE 256

namespace AwsMock::Manager {

    /**
     * @brief Main application class for the awsmock manager.
     *
     * The manager is controlling the different services. Services are activated ib the configuration file or via environment variables. Only the activated
     * services are started. The general flow is:
     *   - Process command line parameters
     *   - Read the configuration file and process environment variables
     *   - Start the database (either MongoDB or in-memory database)
     *   - General initializations
     *   - Start the activated services are background threads
     *   - Start the API gateway on port 4566 by default (can be changed in the configuration file)
     *   - Start the frontend server on port 4567 by default (can be changed in the configuration file)
     *   - Wait for a termination signal
     *
     * @author jens.vogt@opitz-consulting.com
     */
    class Manager {

      public:

        Manager() = default;

        /**
         * @brief Initialization
         */
        static void Initialize();

        /**
         * @brief Stops all currently running modules.
         */
        static void StopModules();

        /**
         * @brief Main processing loop.
         */
        void Run();

      private:

        /**
         * @brief Initialize database
         */
        static void InitializeDatabase();

        /**
         * @brief Load the modules from the configuration file.
         *
         * @par
         * Gateway and monitoring are a bit special, as they are not modules, but they still exists in the module database.
         */
        static void LoadModulesFromConfiguration();

        /**
         * @brief Ensures that the modules exists
         *
         * @param key module key
         */
        static void EnsureModuleExisting(const std::string &key);

        /**
         * Thread group
         */
        boost::thread_group _threadGroup;
    };

}// namespace AwsMock::Manager

#endif//AWSMOCK_MANAGER_H
