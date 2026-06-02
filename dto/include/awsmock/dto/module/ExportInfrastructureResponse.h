//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_RESPONSE_H
#define AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_RESPONSE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/module/model/ExportType.h>
#include <awsmock/dto/module/model/Infrastructure.h>

namespace Awsmock::Dto::Module {

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
        ExportType exportType;

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

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Module"};
    };

}// namespace Awsmock::Dto::Module

#endif// AWSMOCK_DTO_EXPORT_INFRASTRUCTURE_RESPONSE_H
