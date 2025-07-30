//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_METADATA_RESPONSE_H
#define AWSMOCK_DTO_S3_GET_METADATA_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    struct GetMetadataResponse {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * MD5 sum
         */
        std::string md5Sum;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * Size
         */
        long size;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Last modified
         */
        system_clock::time_point modified;

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
        friend std::ostream &operator<<(std::ostream &os, const GetMetadataResponse &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_METADATA_RESPONSE_H
