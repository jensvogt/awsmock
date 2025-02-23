//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_MONITORING_GET_MULTI_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_MONITORING_GET_MULTI_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/entity/monitoring/Counter.h>

namespace AwsMock::Dto::Monitoring {

    /**
     * @brief Get counters response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetMultiCountersResponse {

        /**
         * Counters
         */
        std::map<std::string, std::vector<Database::Entity::Monitoring::Counter>> counters;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const GetMultiCountersResponse &r);
    };

}// namespace AwsMock::Dto::Monitoring

#endif// AWSMOCK_DTO_MONITORING_GET_COUNTERS_RESPONSE_H
