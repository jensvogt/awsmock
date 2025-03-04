//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_PORT_H
#define AWSMOCK_DTO_DOCKER_PORT_H

// C++ includes
#include <string>

// AwsMock includes
#include "awsmock/core/BsonUtils.h"
#include "awsmock/core/LogStream.h"
#include "awsmock/core/exception/JsonException.h"

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker port
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Port {

        /**
         * @brief Constructor
         */
        //explicit Port(const view_or_value<view, value> &object);

        /**
         * Host IP address
         */
        std::string hostIp;

        /**
         * Private port, means port inside the container
         */
        int privatePort{};

        /**
         * Public port, means port visible from docker host
         */
        int publicPort{};

        /**
         * Port type (tcp,udp, etc.)
         */
        std::string type;

        /**
         * @brief Convert to a JSON string
         *
         * @param object JSON object
         */
        void FromDocument(const view_or_value<view, value> &object);

        /**
         * @brief Convert to a JSON string
         *
         * @return object JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Port &c);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_PORT_H
