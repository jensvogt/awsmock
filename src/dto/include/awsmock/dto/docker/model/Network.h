//
// Created by JVO on 22.04.2024.
//

#ifndef AWSMOCK_DTO_DOCKER_NETWORK_H
#define AWSMOCK_DTO_DOCKER_NETWORK_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include "awsmock/core/JsonUtils.h"
#include "awsmock/core/LogStream.h"
#include "awsmock/core/exception/JsonException.h"

namespace AwsMock::Dto::Docker {

    using std::chrono::system_clock;

    /**
     * @brief Docker network object
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Network {

        /**
         * Image ID
         */
        std::string id;

        /**
         * Name
         */
        std::string name;

        /**
         * Driver
         */
        std::string driver;

        /**
         * Scope
         */
        std::string scope;

        /**
         * IPv6 enabled
         */
        bool ipv6Enabled;

        /**
         * Created date time
         */
        system_clock::time_point created;

        /**
         * Convert to a JSON string
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @param jsonObject JSON object
         */
        void FromJson(Poco::JSON::Object::Ptr jsonObject);

        /**
         * Convert to a JSON object
         *
         * @return object JSON object
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

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
        friend std::ostream &operator<<(std::ostream &os, const Network &i);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_NETWORK_H
