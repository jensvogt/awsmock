//
// Created by vogje01 on 10/23/23.
//

#ifndef AWSMOCK_DTO_MODULE_GATEWAY_CONFIG_H
#define AWSMOCK_DTO_MODULE_GATEWAY_CONFIG_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Dto::Module {

    /**
     * Gateway configuration
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GatewayConfig {

        /**
         * Default region
         */
        std::string region;

        /**
         * Endpoint
         */
        std::string endpoint;

        /**
         * Protocol
         */
        std::string protocol = "http";

        /**
         * Host
         */
        std::string host = "localhost";

        /**
         * Listening address
         */
        std::string address = "0.0.0.0";

        /**
         * Port
         */
        int port = 4566;

        /**
         * PID
         */
        int pid = Core::SystemUtils::GetPid();

        /**
         * User ID
         */
        std::string user = "none";

        /**
         * Access ID
         */
        std::string accessId = "000000000000";

        /**
         * Client ID
         */
        std::string clientId = "000000000000";

        /**
         * Access key ID
         */
        std::string accessKeyId = "none";

        /**
         * Secret access key
         */
        std::string secretAccessKey = "none";

        /**
         * Data directory
         */
        std::string dataDir = "/tmp/awsmock/data";

        /**
         * Version
         */
        std::string version;

        /**
         * Pretty print
         */
        bool prettyPrint = false;

        /**
         * Database active
         */
        bool databaseActive = false;

        /**
         * Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Convert from JSON representation
         *
         * @param payload JSON representation
         * @return GatewayConfig
         */
        static GatewayConfig FromJson(const std::string &payload);

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @param os output stream
         * @param m module struct
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const GatewayConfig &m);
    };

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_MODULE_GATEWAY_CONFIG_H
