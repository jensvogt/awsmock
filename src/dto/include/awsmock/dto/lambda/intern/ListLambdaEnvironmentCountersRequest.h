//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_ENVIRONMENT_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_ENVIRONMENT_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>


// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::Lambda {

    struct ListLambdaEnvironmentCountersRequest {

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * Sort column
         */
        std::vector<Database::SortColumn> sortColumns;

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const ListLambdaEnvironmentCountersRequest &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_ENVIRONMENT_COUNTERS_REQUEST_H
