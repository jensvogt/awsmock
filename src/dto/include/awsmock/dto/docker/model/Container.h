//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CONTAINER_H
#define AWSMOCK_DTO_DOCKER_CONTAINER_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/docker/model/Port.h>

#define LAMBDA_INTERNAL_PORT 8080

namespace AwsMock::Dto::Docker {

    /**
     * Docker container
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Container {

        /**
         * Image ID
         */
        std::string id;

        /**
         * Container names
         */
        std::vector<std::string> names;

        /**
         * Image
         */
        std::string image;

        /**
         * Image ID
         */
        std::string imageId;

        /**
         * Command
         */
        std::string command;

        /**
         * The state of this container (e.g. Exited)
         */
        std::string state;

        /**
         * Additional human-readable status of this container (e.g. Exit 0)
         */
        std::string status;

        /**
         * The ports exposed by this container
         */
        std::vector<Port> ports;

        /**
         * The size of files that have been created or changed by this container
         */
        long sizeRw = 0;

        /**
         * The total size of all the files in this container
         */
        long sizeRootFs = 0;

        /**
         * @brief Returns the lambda host port. The lambda host port is the public port for which the private port is 8080.
         *
         * @return lambda public port, or -1 if port does not exist.
         */
        int GetLambdaPort();

        /**
         * @brief Convert to a JSON string
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert from a JSON string
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const Container &c);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CONTAINER_H
