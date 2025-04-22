//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_SERVER_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_LIST_SERVER_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::Transfer {

    struct ListServerCountersRequest {

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
         * Sorting
         */
        std::vector<Database::SortColumn> sortColumns;

        /**
         * @brief Parse a JSON stream
         *
         * @param jsonString json input stream
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Creates a JSON string from the object.
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
        friend std::ostream &operator<<(std::ostream &os, const ListServerCountersRequest &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_SERVER_COUNTERS_REQUEST_H
