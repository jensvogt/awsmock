//
// Created by vogje01 on 2/11/25.
//

#ifndef AWSMOCK_DTO_DOCKER_PORT_BINDING_H
#define AWSMOCK_DTO_DOCKER_PORT_BINDING_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/docker/model/Port.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker port binding object
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PortBinding final : Common::BaseCounter<PortBinding> {

        /**
         * Port map
         */
        std::map<std::string, std::vector<Port>> portMap;

        /**
         * @brief Returns the first public port for a given private port.
         *
         * @param privatePort private port
         * @return public port
         */
        int GetFirstPublicPort(const std::string &privatePort) {
            std::vector<Port> ports = portMap[privatePort];
            if (ports.empty()) {
                ports = portMap[privatePort + "/tcp"];
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
         * @param publicPort public port
         * @return private port
         */
        int GetFirstPrivatePort(const std::string &publicPort) {
            return portMap[publicPort][0].hostPort;
        }

      private:

        friend PortBinding tag_invoke(boost::json::value_to_tag<PortBinding>, boost::json::value const &v) {
            PortBinding r;
            if (Core::Json::AttributeExists(v, "PortMap")) {
                r.portMap = boost::json::value_to<std::map<std::string, std::vector<Port>>>(v.at("PortMap"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PortBinding const &obj) {
            jv = {};
            for (const auto &[fst, snd]: obj.portMap) {
                jv.as_object().emplace(fst, boost::json::value_from(snd));
            }
        }
    };
};// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_PORT_BINDING_H
