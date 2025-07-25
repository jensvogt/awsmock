//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_CREATE_BUCKET_CONSTRAINT_H
#define AWSMOCK_DTO_S3_CREATE_BUCKET_CONSTRAINT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct CreateBucketConstraint {

        /**
         * Bucket location
         */
        std::string location;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert to XML representation
         *
         * Example:
         * @code{.xml}
         * <CreateBucketConfiguration xmlns="http://s3.amazonaws.com/doc/2006-03-01/">
         *     <LocationConstraint>eu-central-1</LocationConstraint>
         * </CreateBucketConfiguration>
         * @endcode
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const;

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
        friend std::ostream &operator<<(std::ostream &os, const CreateBucketConstraint &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_CREATE_BUCKET_CONSTRAINT_H
