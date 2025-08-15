//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_CREATE_MULTIPART_UPLOAD_RESULT_H
#define AWSMOCK_DTO_S3_CREATE_MULTIPART_UPLOAD_RESULT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct CreateMultipartUploadResult {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Key name
         */
        std::string key;

        /**
         * Upload ID
         */
        std::string uploadId;

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
         * Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const CreateMultipartUploadResult &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_CREATE_MULTIPART_UPLOAD_RESULT_H
