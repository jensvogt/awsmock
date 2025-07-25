//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::Docker {

    /**
     * Create container response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateContainerResponse {

        /**
         * ID
         */
        std::string id;

        /**
         * Warnings
         */
        std::vector<std::string> warnings;

        /**
         * Assigned random port
         */
        int hostPort;

        /**
         * Convert to a JSON string
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const CreateContainerResponse &r);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CREATE_CONTAINER_RESPONSE_H
