//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_COUNTER_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/SortColumn.h>

namespace AwsMock::Dto::S3 {

    struct ListBucketCounterRequest {

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
         * Skip
         */
        int pageIndex;

        /**
         * @brief List of sort columns names
         */
        std::vector<Core::SortColumn> sortColumns;

        /**
         * @brief Parse values from a JSON stream
         *
         * @param body json input stream
         */
        static ListBucketCounterRequest FromJson(const std::string &body);

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
        friend std::ostream &operator<<(std::ostream &os, const ListBucketCounterRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_LIST_BUCKET_REQUEST_H
