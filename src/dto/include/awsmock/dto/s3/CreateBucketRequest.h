//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_S3_CREATE_BUCKET_REQUEST_H
#define AWSMOCK_CORE_DTO_S3_CREATE_BUCKET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct CreateBucketRequest {

        /**
         * Bucket location
         */
        std::string region{};

        /**
         * Bucket name
         */
        std::string name{};

        /**
         * Bucket owner
         */
        std::string owner{};

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert from XML representation
         *
         * @param xmlString XML string
         */
        void FromXml(const std::string &xmlString);

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
        friend std::ostream &operator<<(std::ostream &os, const CreateBucketRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_S3_CREATE_BUCKET_REQUEST_H
