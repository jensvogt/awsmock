//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/lambda/model/FunctionCounter.h>

namespace AwsMock::Dto::Lambda {

    struct ListFunctionCountersResponse {

        /**
         * List of buckets
         */
        FunctionCounterList functionCounters;

        /**
         * Total number of buckets
         */
        long total = 0;

        /**
         * @brief Convert to JSON representation
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
        friend std::ostream &operator<<(std::ostream &os, const ListFunctionCountersResponse &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H
