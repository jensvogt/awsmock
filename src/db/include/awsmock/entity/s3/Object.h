//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_OBJECT_H
#define AWSMOCK_DB_ENTITY_S3_OBJECT_H

#ifdef _WIN32
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief S3 object entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Object final : Common::BaseEntity<Object> {

        /**
         * ID
         */
        std::string oid;

        /**
         * Aws region name
         */
        std::string region;

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Bucket ARN
         */
        std::string bucketArn;

        /**
         * Object key
         */
        std::string key;

        /**
         * Object owner
         */
        std::string owner;

        /**
         * Object size
         */
        long size;

        /**
         * Object MD5Sum
         */
        std::string md5sum;

        /**
         * Object sha1Sum
         */
        std::string sha1sum;

        /**
         * Object sha256Sum
         */
        std::string sha256sum;

        /**
         * Object content type
         */
        std::string contentType;

        /**
         * Metadata
         */
        std::map<std::string, std::string> metadata;

        /**
         * Object internal name
         */
        std::string internalName;

        /**
         * Object version ID
         */
        std::string versionId;

        /**
         * Object local name for import
         */
        std::string localName;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        void FromDocument(const std::optional<view> &mResult);
    };

}// namespace AwsMock::Database::Entity::S3

#endif//AWSMOCK_DB_ENTITY_S3_OBJECT_H
