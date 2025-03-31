//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_STATE_H
#define AWSMOCK_DTO_DOCKER_STATE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

#define LAMBDA_INTERNAL_PORT 8080

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container state
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct State {

        /**
         * Error
         */
        std::string error;

        /**
         * Error code
         */
        int errorCode;

        /**
         * PID
         */
        int pid;

        /**
         * Running state
         */
        bool running;

        /**
         * Dead state
         */
        bool dead;

        /**
         * Paused state
         */
        bool paused;

        /**
         * Restarting state
         */
        bool restarting;

        /**
         * OOM killed state
         */
        bool oomKilled;

        /**
         * Status
         */
        std::string status;

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
        friend std::ostream &operator<<(std::ostream &os, const State &c);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_STATE_H
