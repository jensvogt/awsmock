//
// Created by vogje01 on 06/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_BUCKET_ENCRYPTION_H
#define AWSMOCK_DB_ENTITY_S3_BUCKET_ENCRYPTION_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief S3 bucket encryption entity.
     *
     * Saves the bucket encryption parameters
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct BucketEncryption final : Common::BaseEntity<BucketEncryption> {

        /**
         * SSE algorithm
         */
        std::string sseAlgorithm;

        /**
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_BUCKET_ENCRYPTION_H
