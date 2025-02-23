//
// Created by vogje01 on 2/11/25.
//

#ifndef AWSMOCK_DTO_DOCKER_PORT_BINDING_H
#define AWSMOCK_DTO_DOCKER_PORT_BINDING_H


// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/docker/model/Port.h>

namespace AwsMock::Dto::Docker {

    using std::chrono::system_clock;

    /**
     * @brief Docker port binding object
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PortBinding {

        /**
         * Port map
         */
        std::map<std::string, std::vector<Port>> portMap;

        /**
         * @brief Returns the first public port for a given private port.
         *
         * @param privatePort private port
         * @return
         */
        int GetFirstPublicPort(const int privatePort) {
            return portMap[std::to_string(privatePort)].at(0).publicPort;
        };
    };
}// namespace AwsMock::Dto::Docker
#endif// AWSMOCK_DTO_DOCKER_PORT_BINDING_H
