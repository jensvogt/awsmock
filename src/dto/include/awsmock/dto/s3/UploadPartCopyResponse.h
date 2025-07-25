//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_RESPONSE_H
#define AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief Upload part copy reponse
     *
     * Example:
     * @code{.json}
     * <CopyPartResult>
     *   <ETag>string</ETag>
     *   <LastModified>timestamp</LastModified>
     *   <ChecksumCRC32>string</ChecksumCRC32>
     *   <ChecksumCRC32C>string</ChecksumCRC32C>
     *   <ChecksumSHA1>string</ChecksumSHA1>
     *   <ChecksumSHA256>string</ChecksumSHA256>
     * </CopyPartResult>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UploadPartCopyResponse {

        /**
         * ETag
         */
        std::string eTag;

        /**
         * Last modified timestamp
         */
        system_clock::time_point lastModified;

        /**
         * Checksum CRC32
         */
        std::string checksumCRC32 = {};

        /**
         * Checksum CRC32C
         */
        std::string checksumCRC32C = {};

        /**
         * Checksum SHA1
         */
        std::string checksumSHA1 = {};

        /**
         * Checksum SHA256
         */
        std::string checksumSHA256 = {};

        /**
         * Convert to an XML string
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const;

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
        friend std::ostream &operator<<(std::ostream &os, const UploadPartCopyResponse &p);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_RESPONSE_H
