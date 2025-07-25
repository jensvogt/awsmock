//
// Created by JVO on 22.04.2024.
//

#ifndef AWSMOCK_DTO_DOCKER_LOG_CONFIG_H
#define AWSMOCK_DTO_DOCKER_LOG_CONFIG_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/docker/model/PortBinding.h>

namespace AwsMock::Dto::Docker {

    using std::chrono::system_clock;

    /**
     * @brief Docker log config object
     *
     * @par
     * Currently not used.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Config {};

    /**
     * @brief Docker log config object
     *
     * @code{.jso}
     * "LogConfig" : {
     *   "Type" : "json-file",
     *   "Config" : { }
     * }
     * @endCode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LogConfig {

        /**
         * Network mode
         */
        std::string type = "journald";

        /**
         * Network mode
         */
        Config config;

        /**
         * Convert to a JSON string
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * Convert to a JSON object
         *
         * @return object JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const LogConfig &i);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_HOST_CONFIG_H
