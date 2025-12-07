//
// Created by vogje01 on 10/23/23.
//

#ifndef AWSMOCK_DTO_MODULE_GATEWAY_CONFIG_H
#define AWSMOCK_DTO_MODULE_GATEWAY_CONFIG_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Module {

    /**
     * @brief Gateway configuration
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GatewayConfig final : Common::BaseCounter<GatewayConfig> {

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

      private:

        friend GatewayConfig tag_invoke(boost::json::value_to_tag<GatewayConfig>, boost::json::value const &v) {
            GatewayConfig r;
            r.endpoint = Core::Json::GetStringValue(v, "endpoint");
            r.protocol = Core::Json::GetStringValue(v, "protocol");
            r.host = Core::Json::GetStringValue(v, "host");
            r.address = Core::Json::GetStringValue(v, "address");
            r.port = Core::Json::GetIntValue(v, "port");
            r.pid = Core::Json::GetIntValue(v, "pid");
            r.user = Core::Json::GetStringValue(v, "user");
            r.accessId = Core::Json::GetStringValue(v, "accessId");
            r.clientId = Core::Json::GetStringValue(v, "clientId");
            r.accessKeyId = Core::Json::GetStringValue(v, "accessKeyId");
            r.secretAccessKey = Core::Json::GetStringValue(v, "secretAccessKey");
            r.dataDir = Core::Json::GetStringValue(v, "dataDir");
            r.version = Core::Json::GetStringValue(v, "version");
            r.prettyPrint = Core::Json::GetBoolValue(v, "prettyPrint");
            r.databaseActive = Core::Json::GetBoolValue(v, "databaseActive");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GatewayConfig const &obj) {
            jv = {
                    {"endpoint", obj.endpoint},
                    {"protocol", obj.protocol},
                    {"host", obj.host},
                    {"address", obj.address},
                    {"port", obj.port},
                    {"pid", obj.pid},
                    {"user", obj.user},
                    {"accessId", obj.accessId},
                    {"clientId", obj.clientId},
                    {"accessKeyId", obj.accessKeyId},
                    {"secretAccessKey", obj.secretAccessKey},
                    {"dataDir", obj.dataDir},
                    {"version", obj.version},
                    {"prettyPrint", obj.prettyPrint},
                    {"databaseActive", obj.databaseActive},
            };
        }
    };

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_MODULE_GATEWAY_CONFIG_H
