//
// Created by vogje01 on 1/5/25.
//

#ifndef AWSMOCK_SERVICE_TEST_BASE_H
#define AWSMOCK_SERVICE_TEST_BASE_H

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <awsmock/core/config/Configuration.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define AWS_CMD "/usr/local/bin/aws"
#define TEST_IMAGE_NAME std::string("jensvogt/awsmock-test")
#define TEST_CONTAINER_VERSION std::string("latest")
#define TEST_CONTAINER_NAME std::string("awsmock-test")

namespace AwsMock::Service {

    class TestBase {

      public:

        /**
         * @brief Starts the gateway server
         */
        void StartGateway();

        /**
         * @brief Starts the gateway server on a given port
         *
         * @param port gateway port
         */
        void StartGateway(int port);

        /**
         * @brief Stops the gateway server
         */
        void StopGateway();

        /**
         * @brief Returns the current region.
         *
         * @return current region
         */
        std::string GetRegion() { return _region; }

        /**
         * @brief Returns the current endpoint.
         *
         * @return current endpoint
         */
        std::string GetEndpoint() { return _endpoint; }

        static bool InitUnitTests();

        static void InitializeDatabase();

        static void InitializeShm();

        static void StartContainer();

      protected:

        /**
         * Sends a GET HTTP request to the given URL
         * @param url HTTP URL
         * @param payload message body
         * @param port HTTP port
         * @return response struct
         */
        static Core::HttpSocketResponse SendGetCommand(const std::string &url, const std::string &payload, int port);

        /**
         * Sends a PUT HTTP request to the given URL
         * @param url HTTP URL
         * @param payload message body
         * @param port HTTP port
         * @return response struct
         */
        static Core::HttpSocketResponse SendPutCommand(const std::string &url, const std::string &payload, int port);

        /**
         * Sends a POST HTTP request to the given URL
         * @param url HTTP URL
         * @param payload message body
         * @param port HTTP port
         * @return response struct
         */
        static Core::HttpSocketResponse SendPostCommand(const std::string &url, const std::string &payload, int port);

        /**
         * Sends a DELETE HTTP request to the given URL
         * @param url HTTP URL
         * @param payload message body
         * @param port HTTP port
         * @return response struct
         */
        static Core::HttpSocketResponse SendDeleteCommand(const std::string &url, const std::string &payload, int port);

      private:

        boost::thread _thread;
        std::string _endpoint, _region;
        boost::asio::io_context _ios{5};
        std::shared_ptr<GatewayServer> _gatewayServer;
        Core::Configuration &_configuration = Core::Configuration::instance();
    };
}// namespace AwsMock::Service
#endif// AWSMOCK_SERVICE_TEST_BASE_H