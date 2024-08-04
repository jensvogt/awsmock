//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_ABSTRACT_SERVER_H
#define AWSMOCK_SERVICE_ABSTRACT_SERVER_H

// C++ standard includes
#include <iostream>
#include <map>
#include <string>
#include <utility>

// Poco includes
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerResponse.h>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Timer.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/entity/module/ModuleState.h>
#include <awsmock/repository/ModuleDatabase.h>

namespace AwsMock::Service {

    /**
     * @brief Abstract HTTP request server
     *
     * Base class for all server processes.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AbstractServer : public Core::Timer {

      public:

        /**
         * @brief Constructor
         *
         * @param name manager name
         * @param timeout run timeout in seconds
         */
        explicit AbstractServer(std::string name, int timeout);

        /**
         * @brief Constructor
         *
         * @param name manager name
         */
        explicit AbstractServer(std::string name);

        /**
         * @brief Start the HTTP manager
         *
         * @param maxQueueLength maximal request queue length
         * @param maxThreads maximal number of worker threads
         * @param requestTimeout request timeout in seconds
         * @param host HTTP host name
         * @param port HTTP port
         * @param requestFactory HTTP request factory
         */
        void StartHttpServer(int maxQueueLength, int maxThreads, int requestTimeout, const std::string &host, int port, Poco::Net::HTTPRequestHandlerFactory *requestFactory);

        /**
         * @brief Stops the HTTP manager
         */
        void StopHttpServer();

      protected:

        /**
         * @brief Checks whether the module is active
         *
         * @param name module name
         */
        bool IsActive(const std::string &name);

        /**
         * @brief Sets the running status in the module database
         */
        void SetRunning();

        /**
         * @brief Main loop
         */
        void Run() override = 0;


        /**
         * @brief Stop the server
         */
        void Shutdown() override;

      private:

        /**
         * Service name
         */
        std::string _name;

        /**
         * Service database
         */
        Database::ModuleDatabase &_moduleDatabase;

        /**
         * Shutdown mutex
         */
        Poco::Mutex _mutex;

        /**
         * HTTP manager instance
         */
        std::shared_ptr<Poco::Net::HTTPServer> _httpServer;
    };

    /**
     * Server map
     */
    typedef std::map<std::string, std::shared_ptr<Service::AbstractServer>> ServerMap;

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_ABSTRACT_SERVER_H
