//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_RESPONSE_H
#define AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_RESPONSE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/module/model/Infrastructure.h>

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
    struct ExportInfrastructureResponse {

        /**
         * Modules
         */
        Infrastructure infrastructure;

        /**
         * Include objects
         */
        bool includeObjects = false;

        /**
         * Pretty print
         */
        bool prettyPrint = false;

        /**
         * @brief Convert from a JSON object.
         *
         * @return payload json string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString();

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, ExportInfrastructureResponse &r);
    };

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_RESPONSE_H
