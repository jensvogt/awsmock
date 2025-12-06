//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_DTO_IMPORT_INFRASTRUCTURE_REQUEST_H
#define AWSMOCK_DTO_IMPORT_INFRASTRUCTURE_REQUEST_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include "awsmock/dto/common/BaseDto.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/module/model/Infrastructure.h>

namespace AwsMock::Dto::Module {

    /**
     * @brief Import infrastructure request
     *
     * Example:
     * @code{.json}
     * {
     *   "infrastructure": {infrastructure},
     *   "includeObjects": bool,
     *   "cleanFirst": bool
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ImportInfrastructureRequest final : Common::BaseDto<ImportInfrastructureRequest> {

        /**
         * Infrastructure JSON
         */
        Infrastructure infrastructure;

        /**
         * Include objects, default: false
         */
        bool includeObjects = false;

        /**
         * Clean infrastructure first, default: false
         */
        bool cleanFirst = false;

        /**
         * @brief Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Module

#endif//AWSMOCK_DTO_IMPORT_INFRASTRUCTURE_REQUEST_H
