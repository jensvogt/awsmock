//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_COMPLETE_MULTIPART_UPLOAD_RESULT_H
#define AWSMOCK_DTO_S3_COMPLETE_MULTIPART_UPLOAD_RESULT_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct CompleteMultipartUploadResult {

        /**
         * AWS location
         */
        std::string location;

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Key name
         */
        std::string key;

        /**
         * ETag
         */
        std::string etag;

        /**
         * MD5 sum
         */
        std::string md5sum;

        /**
         * Checksum CRC32
         */
        std::string checksumCrc32;

        /**
         * Checksum CRC32C
         */
        std::string checksumCrc32c;

        /**
         * Checksum SHA1
         */
        std::string checksumSha1;

        /**
         * Checksum SHA256
         */
        std::string checksumSha256;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const CompleteMultipartUploadResult &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_COMPLETE_MULTIPART_UPLOAD_RESULT_H
