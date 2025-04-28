//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H
#define AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::Module {

    /**
     * @brief Export infrastructure request
     *
     * Example:
     * @code{.json}
     * {
     *   "modules": ["string", ...],
     *   "onlyObjects": bool,
     *   "prettyPrint": bool,
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ExportInfrastructureRequest {

        /**
         * Modules
         */
        std::vector<std::string> modules;

        /**
         * Include objects, default: false
         */
        bool includeObjects = false;

        /**
         * Pretty print, default: true
         */
        bool prettyPrint = true;

        /**
         * Clean infrastructure first, default: false
         */
        bool cleanFirst = false;

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
        friend std::ostream &operator<<(std::ostream &os, const ExportInfrastructureRequest &r);
    };

}// namespace AwsMock::Dto::Module

#endif//AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_REQUEST_H
