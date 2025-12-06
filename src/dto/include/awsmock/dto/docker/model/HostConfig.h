//
// Created by JVO on 22.04.2024.
//

#ifndef AWSMOCK_DTO_DOCKER_HOST_CONFIG_H
#define AWSMOCK_DTO_DOCKER_HOST_CONFIG_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/docker/model/LogConfig.h>
#include <awsmock/dto/docker/model/PortBinding.h>

namespace AwsMock::Dto::Docker {

    using std::chrono::system_clock;

    /**
     * @brief Docker host config object
     *
     * @code{.jso}
     * "HostConfig" : {
     *   "Binds" : null,
     *   "ContainerIDFile" : "",
     *   "LogConfig" : {
     *     "Type" : "json-file",
     *     "Config" : { }
     *   },
     *   "NetworkMode" : "local",
     *   "PortBindings" : {
     *     "8080" : [{
     *       "HostIp" : "",
     *       "HostPort" : "65519"
     *     }
     *   ]
     * }
     * @endCode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct HostConfig final : Common::BaseCounter<HostConfig> {

        /**
         * Network mode
         */
        std::string networkMode = "local";

        /**
         * Port bindings
         */
        // TODO: cleanup
        std::map<std::string, std::vector<Port>> portBindings;
        //PortBinding portBindings;

        /**
         * Log config
         */
        LogConfig logConfig;

        /**
         * Extra host entries
         */
        std::vector<std::string> extraHosts;

        /**
         * @brief Returns the first public port for a given private port.
         *
         * @param privatePort private port
         * @return public port
         */
        int GetFirstPublicPort(const std::string &privatePort) {
            if (privatePort.empty()) {
                return -1;
            }
            std::vector<Port> ports = portBindings[privatePort];
            if (ports.empty()) {
                ports = portBindings[privatePort + "/tcp"];
            }
            const auto it = std::ranges::find_if(ports, [](const Port &port) {
                return port.hostPort > 0;
            });
            if (it != ports.end()) {
                return it->hostPort;
            }
            return -1;
        }

        /**
         * @brief Returns the first private port for a given public port.
         *
         * @return first private port
         */
        [[nodiscard]] int GetFirstPrivatePort() const {
            if (portBindings.empty()) {
                return -1;
            }
            const std::string firstPrivatePort = portBindings.begin()->first;
            if (firstPrivatePort.empty()) {
                return -1;
            }
            if (Core::StringUtils::EndsWith(firstPrivatePort, "/tcp")) {
                return stoi(firstPrivatePort.substr(0, firstPrivatePort.size() - 4));
            }
            return std::stoi(firstPrivatePort);
        }

      private:

        friend HostConfig tag_invoke(boost::json::value_to_tag<HostConfig>, boost::json::value const &v) {
            HostConfig r = {};
            r.networkMode = Core::Json::GetStringValue(v, "NetworkMode");
            if (Core::Json::AttributeExists(v, "PortBindings")) {
                r.portBindings = boost::json::value_to<std::map<std::string, std::vector<Port>>>(v.at("PortBindings"));
            }
            if (Core::Json::AttributeExists(v, "LogConfig")) {
                r.logConfig = boost::json::value_to<LogConfig>(v.at("LogConfig"));
            }
            if (Core::Json::AttributeExists(v, "ExtraHosts")) {
                r.extraHosts = boost::json::value_to<std::vector<std::string>>(v.at("ExtraHosts"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, HostConfig const &obj) {
            jv = {
                    {"NetworkMode", boost::json::value_from(obj.networkMode)},
                    {"PortBindings", boost::json::value_from(obj.portBindings)},
                    {"LogConfig", boost::json::value_from(obj.logConfig)},
                    {"ExtraHosts", boost::json::value_from(obj.extraHosts)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_HOST_CONFIG_H
