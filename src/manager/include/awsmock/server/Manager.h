//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_MANAGER_H
#define AWSMOCK_MANAGER_H

// Boost includes
#include <boost/interprocess/interprocess_fwd.hpp>

// AwsMock includes
#include <awsmock/service/apps/ApplicationServer.h>
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

namespace AwsMock::Manager {

    /**
     * @brief Main application class for the awsmock manager.
     *
     * @par
     * The manager is controlling the different services. Services are activated in the configuration file or via environment variables. Only the activated
     * services are started. The general flow is:
     *   - Process command line parameters
     *   - Read the configuration file and process environment variables
     *   - Start logging
     *   - Start the database (either MongoDB or in-memory database)
     *   - General initializations
     *   - Start the activated services are background threads
     *   - Auto load infrastructure BSON init files
     *   - Start the API gateway on port 4566 by default (can be changed in the configuration file)
     *   - Start the frontend server on port 4567 by default (can be changed in the configuration file)
     *   - Wait for a termination signal
     *
     * @par
     * Incoming REST request are handles first by the gateway service, which distributed the requests to the different modules. Each request is handled by its
     * own handler thread. In total, there are 256 handler threads available. The module handlers are calling the different service methods on demand.
     *
     * @author jens.vogt@opitz-consulting.com
     */
    class Manager {

      public:

        /**
         * @brief Constructor
         *
         * @param ioc boost IO context
         */
        explicit Manager(boost::asio::io_context &ioc) : _ioc(ioc) {};

        /**
         * @brief Initialization
         */
        void Initialize() const;

        /**
         * @brief Main processing loop.
         */
        void Run(bool isService);

        /**
         * @brief Stop processing-
         */
        static void StopManager() {
            /*Service::ModuleMap moduleMap = Service::ModuleMap::instance();
            StopModules(moduleMap);
            _ioc.stop();*/
        }

        /**
         * @brief Automatically loading the init files in the init directory
         *
         * @par
         * If the server contains a file named /home/awsmock/init/init.json, this file will be imported during startup. If a directory
         * named /home/awsmock/init exists, all files from that directory will be imported. If both exist, the directory gets the
         * precedence. In the case of a directory, all files will be loaded in alphabetic order. So files names 01_xy, 02_xy will be imported
         * in that order.
         */
        static void AutoLoad();

        /**
         * @brief Stops all currently running modules.
         *
         * @param moduleMap hash map of all modules
         */
        static void StopModules(Service::ModuleMap &moduleMap);

      private:

        /**
         * @brief Initialize database
         */
        void InitializeDatabase() const;

        /**
         * @brief Load the modules from the configuration file.
         *
         * @par
         * Gateway and monitoring are a bit special, as they are not modules, but they still exists in the module database.
         */
        static void LoadModulesFromConfiguration();

        /**
         * @brief Ensures that the module exists
         *
         * @param key module key
         */
        static void EnsureModuleExisting(const std::string &key);

        /**
         * Create a shared memory segment for monitoring
         */
        void CreateSharedMemorySegment();

        /**
         * Thread group
         */
        boost::thread_group _threadGroup;

        /**
         * MongoDB connection pool
         */
        Database::ConnectionPool &_pool = Database::ConnectionPool::instance();

        /**
         * Global shared memory segment
         */
        std::unique_ptr<boost::interprocess::managed_shared_memory> _shm;

        /**
         * Boost IO context
         */
        boost::asio::io_context &_ioc;
    };

}// namespace AwsMock::Manager

#endif//AWSMOCK_MANAGER_H
