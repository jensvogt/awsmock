//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_REQUEST_H
#define AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct UploadPartCopyRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Upload ID
         */
        std::string uploadId;

        /**
         * Part number
         */
        int partNumber;

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
         * Min byte from range
         */
        long min;

        /**
         * Max byte from range
         */
        long max;

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
        friend std::ostream &operator<<(std::ostream &os, const UploadPartCopyRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_REQUEST_H
