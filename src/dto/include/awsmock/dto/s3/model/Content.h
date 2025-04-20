//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_S3_CONTENT_H
#define AWSMOCK_DTO_S3_CONTENT_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/s3/model/Owner.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief AWS S3 content DTO.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Content {

        /**
         * Checksum algorithms
         */
        std::vector<std::string> checksumAlgorithms;

        /**
         * ETag
         */
        std::string etag;

        /**
         * Key
         */
        std::string key;

        /**
         * Owner
         */
        Owner owner;

        /**
         * Size
         */
        long size = 0;

        /**
         * Storage class
         */
        std::string storageClass;

        /**
         * Last modified
         */
        system_clock::time_point modified;

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
        friend std::ostream &operator<<(std::ostream &os, const Content &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_CONTENT_H
