//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_MANAGER_H
#define AWSMOCK_MANAGER_H

// Boost includes
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

// AwsMock includes
#include <awsmock/core/SharedMemoryUtils.h>
#include <awsmock/core/logging/LogStream.h>
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
     * The manager is controlling the different services. Services are activated ib the configuration file or via environment variables. Only the activated
     * services are started. The general flow is:
     *   - Process command line parameters
     *   - Read the configuration file and process environment variables
     *   - Start the database (either MongoDB or in-memory database)
     *   - General initializations
     *   - Start the activated services are background threads
     *   - Auto load infrastructure BSON init file
     *   - Start the API gateway on port 4566 by default (can be changed in the configuration file)
     *   - Start the frontend server on port 4567 by default (can be changed in the configuration file)
     *   - Wait for a termination signal
     *
     * @author jens.vogt@opitz-consulting.com
     */
    class Manager {

      public:

        explicit Manager(boost::asio::io_context &ioc) : _ioc(ioc) {};

        /**
         * @brief Initialization
         */
        void Initialize() const;

        /**
         * @brief Main processing loop.
         */
        void Run();

        /**
         * @brief Stop processing-
         */
        void Stop() const { _ioc.stop(); }

        /**
         * @brief Automatically loading the init file
         *
         * @par
         * If the server contains a file named /home/awsmock/init/init.json, this file will be imported during startup. If a directory
         * named /home/awsmock/init exists, all files from that directory will be imported. If both exists, the directory gets the
         * precedence.
         */
        static void AutoLoad();

        /**
         * @brief Stops all currently running modules.
         */
        static void StopModules(Service::ModuleMap &moduleMap);

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

        boost::asio::io_context &_ioc;
    };

}// namespace AwsMock::Manager

#endif//AWSMOCK_MANAGER_H
