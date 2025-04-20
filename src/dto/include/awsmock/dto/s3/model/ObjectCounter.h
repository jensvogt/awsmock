//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_OBJECT_COUNTER_H
#define AWSMOCK_DTO_S3_OBJECT_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief AWS S3 object counter DTO.
     *
     * @par
     * Used only internally by the frontend.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ObjectCounter {

        /**
         * Database OID
         */
        std::string oid;

        /**
         * AWS region
         */
        std::string region;

        /**
         * Bucket name
         */
        std::string bucketName;

        /**
         * Object key
         */
        std::string key;

        /**
         * Content type (mime type)
         */
        std::string contentType;

        /**
         * Object size in bytes
         */
        long size;

        /**
         * Internal file name
         */
        std::string internalName;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Created timestamp
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
        friend std::ostream &operator<<(std::ostream &os, const ObjectCounter &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_OBJECT_COUNTER_H
