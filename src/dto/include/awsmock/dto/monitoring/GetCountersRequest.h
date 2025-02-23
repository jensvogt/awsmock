//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_MONITORING_GET_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_MONITORING_GET_COUNTERS_REQUEST_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::Monitoring {

    using std::chrono::system_clock;

    /**
     * @brief Get counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetCountersRequest {

        /**
         * AWS region name
         */
        std::string region;

        /**
         * Name
         */
        std::string name;

        /**
         * Label name
         */
        std::string labelName;

        /**
         * Label name
         */
        std::string labelValue;

        /**
         * Stepping
         */
        int step;

        /**
         * Limit to top x counters
         */
        int limit = 10;

        /**
         * Start time
         */
        system_clock::time_point start;

        /**
         * End time
         */
        system_clock::time_point end;

        /**
         * @brief Parse values from a JSON stream
         *
         * @param body json input stream
         */
        static GetCountersRequest FromJson(const std::string &body);

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
        friend std::ostream &operator<<(std::ostream &os, const GetCountersRequest &r);
    };

}// namespace AwsMock::Dto::Monitoring

#endif// AWSMOCK_DTO_MONITORING_GET_COUNTERS_REQUEST_H
