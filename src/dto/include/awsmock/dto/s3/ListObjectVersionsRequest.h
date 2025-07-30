//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_REQUEST_H
#define AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief List object version request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListObjectVersionsRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Delimiter
         */
        std::string delimiter;

        /**
         * Encoding type
         */
        std::string encodingType;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Maximal number of keys
         */
        int maxKeys;

        /**
         * Version ID marker
         */
        std::string versionIdMarker;

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
        friend std::ostream &operator<<(std::ostream &os, const ListObjectVersionsRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_OBJECT_VERSIONS_REQUEST_H
