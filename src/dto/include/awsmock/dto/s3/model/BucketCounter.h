//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_BUCKET_COUNTER_H
#define AWSMOCK_DTO_S3_BUCKET_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief AWS S3 bucket counter DTO.
     *
     * @par
     * Used only internally by the frontend.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct BucketCounter {

        /**
         * Bucket name
         */
        std::string bucketName;

        /**
         * Number of objects keys
         */
        long keys;

        /**
         * Bucket size in bytes
         */
        long size;

        /**
         * Bucket owner
         */
        std::string owner;

        /**
         * Create timestamp
         */
        system_clock::time_point created;

        /**
         * Create timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Convert to a BSON document
         *
         * @return BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

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
        friend std::ostream &operator<<(std::ostream &os, const BucketCounter &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_BUCKET_COUNTER_H
