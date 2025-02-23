//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_INSPECT_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_INSPECT_CONTAINER_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include "model/HostConfig.h"
#include "model/PortBinding.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/docker/model/State.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Inspect container response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct InspectContainerResponse {

        /**
         * ID
         */
        std::string id;

        /**
         * Name
         */
        std::string name;

        /**
         * Path
         */
        std::string path;

        /**
         * Image
         */
        std::string image;

        /**
         * State
         */
        State state;

        /**
         * Host config
         */
        HostConfig hostConfig;

        /**
         * @brief Convert to a JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const InspectContainerResponse &r);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_INSPECT_CONTAINER_RESPONSE_H
