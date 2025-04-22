//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::Lambda {

    struct ListFunctionCountersRequest {

        /**
         * Region
         */
        std::string region;

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
         * List of sort columns names
         */
        std::vector<Database::SortColumn> sortColumns;

        /**
         * @brief Parse values from a JSON stream
         *
         * @param body json input stream
         */
        static ListFunctionCountersRequest FromJson(const std::string &body);

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
        friend std::ostream &operator<<(std::ostream &os, const ListFunctionCountersRequest &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_REQUEST_H
