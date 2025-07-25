//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_CLEAN_INFRASTRUCTURE_REQUEST_H
#define AWSMOCK_DTO_CLEAN_INFRASTRUCTURE_REQUEST_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::Module {

    /**
     * @brief Clean infrastructure request
     *
     * Example:
     * @code{.json}
     * {
     *   "modules": ["string", ...],
     *   "onlyObjects": bool,
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CleanInfrastructureRequest {

        /**
         * Modules
         */
        std::vector<std::string> modules;

        /**
         * Only objects
         */
        bool onlyObjects;

        /**
         * Pretty print
         */
        bool prettyPrint;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

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
        friend std::ostream &operator<<(std::ostream &os, const CleanInfrastructureRequest &r);
    };

}// namespace AwsMock::Dto::Module

#endif//AWSMOCK_DTO_CLEAN_INFRASTRUCTURE_REQUEST_H
