//
// Created by vogje01 on 1/5/25.
//

#ifndef AWSMOCK_SERVICE_TEST_BASE_H
#define AWSMOCK_SERVICE_TEST_BASE_H

#include <awsmock/core/config/Configuration.h>
#include <awsmock/service/gateway/GatewayServer.h>

#ifdef __linux__
#define AWS_CMD "/snap/bin/aws"
#elif __APPLE__
#define AWS_CMD "/usr/local/bin/aws"
#endif

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
        boost::asio::io_context _ios{10};
        std::shared_ptr<GatewayServer> _gatewayServer;
        Core::Configuration &_configuration = Core::Configuration::instance();
    };
}// namespace AwsMock::Service
#endif// AWSMOCK_SERVICE_TEST_BASE_H