//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_OBJECT_COUNTER_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_OBJECT_COUNTER_REQUEST_H

// C++ Standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/SortColumn.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct ListObjectCounterRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * MaxResults
         */
        int maxResults;

        /**
         * Skip
         */
        int skip;

        /**
         * List of sort columns names
         */
        std::vector<Core::SortColumn> sortColumns;

        /**
         * Parse values from a JSON stream
         *
         * @param body json input stream
         */
        static ListObjectCounterRequest FromJson(const std::string &body);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListObjectCounterRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_LIST_OBJECT_REQUEST_H