//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_COPY_OBJECT_REQUEST_H
#define AWSMOCK_CORE_DTO_COPY_OBJECT_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct CopyObjectRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Region
         */
        std::string user;

        /**
         * Source bucket
         */
        std::string sourceBucket;

        /**
         * Source key
         */
        std::string sourceKey;

        /**
         * Target bucket
         */
        std::string targetBucket;

        /**
         * Target key
         */
        std::string targetKey;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

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
        friend std::ostream &operator<<(std::ostream &os, const CopyObjectRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_COPY_OBJECT_REQUEST_H
