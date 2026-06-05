//
// Created by vogje01 on 11/19/23.
//

#pragma once

// C++ includes
#include <ranges>
#include <string>
#include <unordered_set>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/Linq.h>
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/PagingUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/repository/s3/IS3Repository.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief S3 in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3MemoryRepository final : public IS3Repository {

      public:

        /**
         * @brief Constructor
         */
        S3MemoryRepository() = default;

        /**
         * @brief Bucket exists
         *
         * @param region AWS region
         * @param name bucket name
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool bucketExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Bucket exists
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool bucketExists(const Entity::S3::Bucket &bucket) const override;

        /**
         * @brief Bucket exists by ARN
         *
         * @param bucketArn AWS ARN
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool bucketExists(const std::string &bucketArn) const override;

        /**
         * @brief Returns the bucket by userPoolId
         *
         * @param oid bucket oid
         * @return bucket, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Bucket getBucketById(const std::string &oid) const override;

        /**
         * @brief Returns the bucket by userPoolId
         *
         * @param oid bucket oid
         * @return bucket, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Bucket getBucketById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns the bucket by region and name.
         *
         * @param region AWS region
         * @param name bucket name
         * @return bucket entity
         */
        [[nodiscard]]
        Entity::S3::Bucket getBucketByRegionName(const std::string &region, const std::string &name) const override;

        /**
         * @brief Returns the bucket by AWS ARN
         *
         * @param bucketArn AWS region
         * @return bucket entity
         */
        [[nodiscard]]
        Entity::S3::Bucket getBucketByArn(const std::string &bucketArn) const override;

        /**
         * @brief Create a new bucket in the S3 bucket table
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Bucket createBucket(Entity::S3::Bucket &bucket) const override;

        /**
         * @brief List all buckets
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param maxResults maximal number of results
         * @param skip number of records to skip
         * @param sortColumns sorting columns
         * @return BucketList
         */
        [[nodiscard]]
        Entity::S3::BucketList listBuckets(const std::string &region, const std::string &prefix, long maxResults, long skip, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Export all buckets
         *
         * @param sortColumns sorting columns
         * @return BucketList
         */
        [[nodiscard]]
        Entity::S3::BucketList exportBuckets(const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Check whether the bucket has still objected
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool hasObjects(const Entity::S3::Bucket &bucket) const override;

        /**
         * @brief List objects in a bucket.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @param maxKeys maximal number of return elements
         * @return list of S3 objects
         */
        [[nodiscard]]
        std::vector<Entity::S3::Object> getBucketObjectList(const std::string &region, const std::string &bucket, long maxKeys) const override;

        /**
         * @brief Count objects in a bucket.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return number of S3 objects
         */
        [[nodiscard]]
        long getBucketObjectCount(const std::string &region, const std::string &bucket) const override;

        /**
         * @brief Sum of all object sizes in bytes.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return size of S3 objects in bytes
         */
        [[nodiscard]]
        long getBucketSize(const std::string &region, const std::string &bucket) const override;

        /**
         * @brief List all objects of a bucket
         *
         * @param bucket S3 bucket name
         * @param prefix S3 key prefix
         * @return ObjectList
         */
        [[nodiscard]]
        std::vector<Entity::S3::Object> listBucket(const std::string &bucket, const std::string &prefix) const override;

        /**
         * @brief Returns the total number of buckets
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return total number of buckets
         * @throws DatabaseException
         */
        [[nodiscard]]
        long bucketCount(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Purges a bucket
         *
         * @param bucket bucket entity
         * @return number of deleted objects
         * @throws DatabaseException
         */
        [[nodiscard]]
        long purgeBucket(Entity::S3::Bucket &bucket) const override;

        /**
         * @brief Updates a bucket
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Bucket updateBucket(Entity::S3::Bucket &bucket) const override;

        /**
         * @brief Updates the counters of a bucket
         *
         * @param bucketArn bucker ARN
         * @param keys number of keys
         * @param size bucket size
         * @return created bucket entity
         * @throws DatabaseException
         */
        void updateBucketCounter(const std::string &bucketArn, long keys, long size) const override;

        /**
         * @brief List all objects.
         *
         * @param region AWS region
         * @param prefix S3 key prefix
         * @param bucket S3 bucket name
         * @param pageSize maximal number of results
         * @param pageIndex page index
         * @param sortColumns list of sort columns
         * @return ObjectList
         */
        [[nodiscard]]
        std::vector<Entity::S3::Object> listObjects(const std::string &region, const std::string &prefix, const std::string &bucket, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Gets a list of versioned objects
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param prefix object key prefix
         * @return list of S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        std::vector<Entity::S3::Object> listObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const override;

        /**
         * @brief Delete a bucket.
         *
         * @param bucket bucket entity
         * @throws DatabaseException
         */
        void deleteBucket(const Entity::S3::Bucket &bucket) const override;

        /**
         * @brief Deletes all buckets
         *
         * @return total number of deleted objects
         */
        [[nodiscard]]
        long deleteAllBuckets() const override;

        /**
         * Check the existence of an object
         *
         * @param object object entity
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool objectExists(const Entity::S3::Object &object) const override;

        /**
         * @brief Check the existence of an object by internal name
         *
         * @param filename object internal name
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool objectExistsInternalName(const std::string &filename) const override;

        /**
         * @brief Check the existence of an object by OID
         *
         * @param oid object ID
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool objectExists(const std::string &oid) const override;

        /**
         * @brief Bucket exists
         *
         * @param region AWS region
         * @param bucket bucket name
         * @param key S3 key
         * @return true if object exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool objectExists(const std::string &region, const std::string &bucket, const std::string &key) const override;

        /**
         * @brief Create a new S3 object in the S3 object table
         *
         * @param object object entity
         * @return created object entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Object createObject(Entity::S3::Object &object) const override;

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param object object entity
         * @return updated object entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Object updateObject(Entity::S3::Object &object) const override;

        /**
         * @brief Gets an object from a bucket
         *
         * @param oid object ID
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Object getObjectById(const std::string &oid) const override;

        /**
         * @brief Gets an object from a bucket
         *
         * @param oid object ID
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Object getObjectById(bsoncxx::oid oid) const override;

        /**
         * @brief Create a new bucket or update an existing bucket
         *
         * @param bucket bucket entity
         * @return created or updated bucket entity
         */
        [[nodiscard]]
        Entity::S3::Bucket createOrUpdateBucket(Entity::S3::Bucket &bucket) const override;

        /**
         * @brief Create a new S3 object or update an existing one
         *
         * @param object object entity
         * @return created or updated object entity
         */
        [[nodiscard]]
        Entity::S3::Object createOrUpdateObject(Entity::S3::Object &object) const override;

        /**
         * @brief Gets an object by version
         *
         * @param region AWS region
         * @param bucket bucket name
         * @param key object key
         * @param version version ID
         * @return S3 object
         */
        [[nodiscard]]
        Entity::S3::Object getObjectVersion(const std::string &region, const std::string &bucket, const std::string &key, const std::string &version) const override;

        /**
         * @brief Creates a bucket notification
         *
         * @param bucket S3 bucket
         * @param bucketNotification bucket notification
         * @return updated Bucket entity
         */
        [[nodiscard]]
        Entity::S3::Bucket createBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const override;

        /**
         * @brief Deletes bucket notifications
         *
         * @param bucket S3 bucket
         * @param bucketNotification bucket notification
         * @return updated Bucket entity
         */
        [[nodiscard]]
        Entity::S3::Bucket deleteBucketNotifications(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const override;

        /**
         * @brief Gets an object from an bucket
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Object getObject(const std::string &region, const std::string &bucket, const std::string &key) const override;

        /**
         * @brief Gets an object from a bucket using the bucket, key and MD5 sum as query parameter
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @param md5sum MD5 sum of object
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::S3::Object getObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum) const override;

        /**
         * @brief Counts the number of keys in a bucket
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param bucket bucket name
         * @return number of objects in the bucket
         * @throws DatabaseException
         */
        [[nodiscard]]
        long objectCount(const std::string &region, const std::string &prefix, const std::string &bucket) const override;

        /**
         * @brief Delete an object.
         *
         * @param object object entity
         * @return number of objects deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteObject(const Entity::S3::Object &object) const override;

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param region AWS region
         * @param bucket bucket to delete from
         * @param keys vector of object keys
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteObjects(const std::string &region, const std::string &bucket, const std::vector<std::string> &keys) const override;

        /**
         * @brief Deletes all objects
         *
         * @retrun number of objects deleted.
         */
        [[nodiscard]]
        long deleteAllObjects() const override;

        /**
         * Recalculate the object counters
         */
        void adjustObjectCounters() const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "S3"};

        /**
         * S3 bucket map when running without a database
         */
        mutable std::unordered_map<std::string, Entity::S3::Bucket> _buckets{};

        /**
         * S3 object map when running without a database
         */
        mutable std::unordered_map<std::string, Entity::S3::Object> _objects{};

        /**
         * Bucket mutex
         */
        static boost::mutex _bucketMutex;

        /**
         * Object mutex
         */
        static boost::mutex _objectMutex;
    };

}// namespace Awsmock::Database
