//
// Created by vogje01 on 13/10/2023.
//

#ifndef AWSMOCK_CORE_DTO_PUT_BUCKET_VERSIONING_REQUEST_H
#define AWSMOCK_CORE_DTO_PUT_BUCKET_VERSIONING_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock include
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>

namespace AwsMock::Dto::S3 {

    struct PutBucketVersioningRequest {

        /**
         * AWS region
         */
        std::string region;

        /**
         * AWS user
         */
        std::string owner;

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Status
         */
        std::string status;

        /**
         * @brief Convert from XML representation
         *
         * @param xmlString XML string
         */
        void FromXml(const std::string &xmlString);

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const PutBucketVersioningRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_PUT_BUCKET_VERSIONING_REQUEST_H
