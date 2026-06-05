//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/MessageWaitTime.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for S3 repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * S3-related data.
     */
    class IS3Repository {

      public:

        /**
         * @brief Virtual destructor for the IS3Repository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~IS3Repository() = default;

        /**
         * @brief Bucket exists
         *
         * @param region AWS region
         * @param name bucket name
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool bucketExists(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Bucket exists
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool bucketExists(const Entity::S3::Bucket &bucket) const = 0;

        /**
         * @brief Bucket exists by ARN
         *
         * @param bucketArn AWS ARN
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool bucketExists(const std::string &bucketArn) const = 0;

        /**
         * @brief Returns the total number of buckets
         *
         * @param region AWS region
         * @param prefix bucket name prefix
         * @return total number of buckets
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long bucketCount(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Returns the bucket by userPoolId
         *
         * @param oid bucket oid
         * @return bucket, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket getBucketById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns the bucket by userPoolId
         *
         * @param oid bucket oid
         * @return bucket, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket getBucketById(const std::string &oid) const = 0;

        /**
         * @brief Returns the bucket by region and name.
         *
         * @param region AWS region
         * @param name bucket name
         * @return bucket entity
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket getBucketByRegionName(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Returns the bucket by AWS ARN
         *
         * @param bucketArn AWS region
         * @return bucket entity
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket getBucketByArn(const std::string &bucketArn) const = 0;

        /**
         * @brief Create a new bucket in the S3 bucket table
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket createBucket(Entity::S3::Bucket &bucket) const = 0;

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
        virtual Entity::S3::BucketList listBuckets(const std::string &region, const std::string &prefix, long maxResults, long skip, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Export all buckets
         *
         * @param sortColumns sorting columns
         * @return BucketList
         */
        [[nodiscard]]
        virtual Entity::S3::BucketList exportBuckets(const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Check whether the bucket has still objects
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool hasObjects(const Entity::S3::Bucket &bucket) const = 0;

        /**
         * @brief List objects in a bucket.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @param maxKeys maximal number of return elements
         * @return list of S3 objects
         */
        [[nodiscard]]
        virtual std::vector<Entity::S3::Object> getBucketObjectList(const std::string &region, const std::string &bucket, long maxKeys) const = 0;

        /**
         * @brief Counts objects in a bucket.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return number of S3 objects
         */
        [[nodiscard]]
        virtual long getBucketObjectCount(const std::string &region, const std::string &bucket) const = 0;

        /**
         * @brief Purges a bucket
         *
         * @param bucket bucket entity
         * @return number of deleted objects
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long purgeBucket(Entity::S3::Bucket &bucket) const = 0;

        /**
         * @brief Updates a bucket
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket updateBucket(Entity::S3::Bucket &bucket) const = 0;

        /**
         * @brief Updates a bucket
         *
         * @param bucketArn bucker ARN
         * @param keys number of keys
         * @param size bucket size
         * @return created bucket entity
         * @throws DatabaseException
         */
        virtual void updateBucketCounter(const std::string &bucketArn, long keys, long size) const = 0;

        /**
         * @brief Returns the total bucket size.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return bucket size in bytes
         */
        [[nodiscard]]
        virtual long getBucketSize(const std::string &region, const std::string &bucket) const = 0;

        /**
         * @brief Create a new bucket or updated a existing bucket
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket createOrUpdateBucket(Entity::S3::Bucket &bucket) const = 0;

        /**
         * @brief Create a new S3 object in the S3 object table
         *
         * @param object object entity
         * @return created object entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Object createObject(Entity::S3::Object &object) const = 0;

        /**
         * @brief Create a new S3 object in the S3 object table if it does not exist, otherwise update the exiting object.
         *
         * @param object object entity
         * @return created or updated object entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Object createOrUpdateObject(Entity::S3::Object &object) const = 0;

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param object object entity
         * @return updated object entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Object updateObject(Entity::S3::Object &object) const = 0;

        /**
         * @brief Check the existence of an object
         *
         * @param object object entity
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool objectExists(const Entity::S3::Object &object) const = 0;

        /**
         * @brief Check the existence of an object by OID
         *
         * @param oid object ID
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool objectExists(const std::string &oid) const = 0;

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
        virtual bool objectExists(const std::string &region, const std::string &bucket, const std::string &key) const = 0;

        /**
         * @brief Check the existence of an object by internal name
         *
         * @param filename object internal name
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool objectExistsInternalName(const std::string &filename) const = 0;

        /**
         * @brief Gets an object from a bucket
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Object getObject(const std::string &region, const std::string &bucket, const std::string &key) const = 0;

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
        virtual Entity::S3::Object getObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum) const = 0;

        /**
         * @brief Gets an object from a bucket using the bucket, key and MD5 sum as query parameter
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @param version version ID
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Object getObjectVersion(const std::string &region, const std::string &bucket, const std::string &key, const std::string &version) const = 0;

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
        virtual std::vector<Entity::S3::Object> listObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const = 0;

        /**
         * @brief Gets an object from a bucket
         *
         * @param oid object ID
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Object getObjectById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Gets an object from a bucket
         *
         * @param oid object ID
         * @return S3 object
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::S3::Object getObjectById(const std::string &oid) const = 0;

        /**
         * @brief List all objects of a bucket
         *
         * @param bucket S3 bucket name
         * @param prefix S3 key prefix
         * @return ObjectList
         */
        [[nodiscard]]
        virtual std::vector<Entity::S3::Object> listBucket(const std::string &bucket, const std::string &prefix) const = 0;

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
        virtual std::vector<Entity::S3::Object> listObjects(const std::string &region, const std::string &prefix, const std::string &bucket, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Counts the number of keys in a bucket
         *
         * @param region AWS region
         * @param prefix key prefix
         * @param bucket bucket name
         * @return number of objects in the bucket
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long objectCount(const std::string &region, const std::string &prefix, const std::string &bucket) const = 0;

        /**
         * @brief Creates a bucket notification.
         *
         * <p>In case of a wildcard notification, all notifications are added.</p>
         *
         * @param bucket S3 bucket
         * @param bucketNotification bucket notification
         * @return updated Bucket entity
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket createBucketNotification(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const = 0;

        /**
         * @brief Creates a bucket notification-
         *
         * @param bucket S3 bucket
         * @param bucketNotification bucket notification
         * @return updated Bucket entity
         */
        [[nodiscard]]
        virtual Entity::S3::Bucket deleteBucketNotifications(const Entity::S3::Bucket &bucket, const Entity::S3::BucketNotification &bucketNotification) const = 0;

        /**
         * @brief Delete a bucket.
         *
         * @param bucket bucket entity
         * @throws DatabaseException
         */
        virtual void deleteBucket(const Entity::S3::Bucket &bucket) const = 0;

        /**
         * @brief Deletes all buckets
         *
         * @return total number of deleted objects
         */
        [[nodiscard]]
        virtual long deleteAllBuckets() const = 0;

        /**
         * @brief Delete an object.
         *
         * @param object object entity
         * @return number of objects deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteObject(const Entity::S3::Object &object) const = 0;

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param region AWS region
         * @param bucketName bucket to delete from
         * @param keys vector of object keys
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteObjects(const std::string &region, const std::string &bucketName, const std::vector<std::string> &keys) const = 0;

        /**
         * @brief Deletes all objects
         *
         * @return number of objects deleted.
         */
        [[nodiscard]]
        virtual long deleteAllObjects() const = 0;

        /**
         * @brief Adjust all object counters
         */
        virtual void adjustObjectCounters() const = 0;
    };

}// namespace Awsmock::Database