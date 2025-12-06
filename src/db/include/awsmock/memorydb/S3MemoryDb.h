//
// Created by vogje01 on 11/19/23.
//

#ifndef AWSMOCK_REPOSITORY_S3_MEMORYDB_H
#define AWSMOCK_REPOSITORY_S3_MEMORYDB_H

// C++ includes
#include <ranges>
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/entity/s3/Bucket.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    /**
     * @brief S3 in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3MemoryDb {

      public:

        /**
         * @brief Constructor
         */
        S3MemoryDb() = default;

        /**
         * @brief Singleton instance
         *
         * @return singleton instance
         */
        static S3MemoryDb &instance() {
            static S3MemoryDb s3MemoryDb;
            return s3MemoryDb;
        }

        /**
         * @brief Bucket exists
         *
         * @param region AWS region
         * @param name bucket name
         * @return true if bucket exists
         * @throws DatabaseException
         */
        bool BucketExists(const std::string &region, const std::string &name);

        /**
         * @brief Bucket exists
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        bool BucketExists(const Entity::S3::Bucket &bucket);

        /**
         * @brief Bucket exists by ARN
         *
         * @param bucketArn AWS ARN
         * @return true if bucket exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool BucketExists(const std::string &bucketArn) const;

        /**
         * @brief Returns the bucket by userPoolId
         *
         * @param oid bucket oid
         * @return bucket, if existing
         * @throws DatabaseException
         */
        Entity::S3::Bucket GetBucketById(const std::string &oid);

        /**
         * @brief Returns the bucket by region and name.
         *
         * @param region AWS region
         * @param name bucket name
         * @return bucket entity
         */
        Entity::S3::Bucket GetBucketByRegionName(const std::string &region, const std::string &name);

        /**
         * @brief Returns the bucket by AWS ARN
         *
         * @param bucketArn AWS region
         * @return bucket entity
         */
        Entity::S3::Bucket GetBucketByArn(const std::string &bucketArn);

        /**
         * @brief Create a new bucket in the S3 bucket table
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        Entity::S3::Bucket CreateBucket(const Entity::S3::Bucket &bucket);

        /**
         *  List all buckets
         *
         * @return BucketList
         */
        Entity::S3::BucketList ListBuckets();

        /**
         * @brief Export all buckets
         *
         * @param sortColumns sorting columns
         * @return BucketList
         */
        Entity::S3::BucketList ExportBuckets(const std::vector<SortColumn> &sortColumns = {}) const;

        /**
         * @brief Check whether the bucket has still objected
         *
         * @param bucket bucket entity
         * @return true if bucket exists
         * @throws DatabaseException
         */
        bool HasObjects(const Entity::S3::Bucket &bucket) const;

        /**
         * @brief List objects in a bucket.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @param maxKeys maximal number of return elements
         * @return list of S3 objects
         */
        std::vector<Entity::S3::Object> GetBucketObjectList(const std::string &region, const std::string &bucket, long maxKeys) const;

        /**
         * @brief Count objects in a bucket.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return number of S3 objects
         */
        long GetBucketObjectCount(const std::string &region, const std::string &bucket) const;

        /**
         * @brief Sum of all object sizes in bytes.
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return size of S3 objects in bytes
         */
        long GetBucketSize(const std::string &region, const std::string &bucket) const;

        /**
         * @brief List all objects of a bucket
         *
         * @param bucket S3 bucket name
         * @param prefix S3 key prefix
         * @return ObjectList
         */
        std::vector<Entity::S3::Object> ListBucket(const std::string &bucket, const std::string &prefix = {}) const;

        /**
         * @brief Returns the total number of buckets
         *
         * @return total number of buckets
         * @throws DatabaseException
         */
        long BucketCount() const;

        /**
         * @brief Purges a bucket
         *
         * @param bucket bucket entity
         * @return number of deleted objects
         * @throws DatabaseException
         */
        long PurgeBucket(const Entity::S3::Bucket &bucket);

        /**
         * @brief Updates a bucket
         *
         * @param bucket bucket entity
         * @return created bucket entity
         * @throws DatabaseException
         */
        Entity::S3::Bucket UpdateBucket(const Entity::S3::Bucket &bucket);

        /**
         * @brief Updates the counters of a bucket
         *
         * @param bucketArn bucker ARN
         * @param keys number of keys
         * @param size bucket size
         * @return created bucket entity
         * @throws DatabaseException
         */
        void UpdateBucketCounter(const std::string &bucketArn, long keys, long size);

        /**
         * @brief List all objects.
         *
         * @param prefix S3 key prefix
         * @return ObjectList
         */
        [[nodiscard]] std::vector<Entity::S3::Object> ListObjects(const std::string &prefix = {}) const;

        /**
         * @brief Gets a list of versioned objects
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param prefix object key prefix
         * @return list of S3 object
         * @throws DatabaseException
         */
        std::vector<Entity::S3::Object> ListObjectVersions(const std::string &region, const std::string &bucket, const std::string &prefix) const;

        /**
         * @brief Delete a bucket.
         *
         * @param bucket bucket entity
         * @throws DatabaseException
         */
        void DeleteBucket(const Entity::S3::Bucket &bucket);

        /**
         * @brief Deletes all buckets
         *
         * @return total number of deleted objects
         */
        long DeleteAllBuckets();

        /**
         * Check the existence of an object
         *
         * @param object object entity
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        bool ObjectExists(const Entity::S3::Object &object);

        /**
         * @brief Check the existence of an object by internal name
         *
         * @param filename object internal name
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        bool ObjectExistsInternalName(const std::string &filename);

        /**
         * @brief Check the existence of an object by OID
         *
         * @param oid object ID
         * @return true if existing otherwise false
         * @throws DatabaseException
         */
        bool ObjectExists(const std::string &oid) const;

        /**
         * @brief Bucket exists
         *
         * @param region AWS region
         * @param bucket bucket name
         * @param key S3 key
         * @return true if object exists
         * @throws DatabaseException
         */
        bool ObjectExists(const std::string &region, const std::string &bucket, const std::string &key) const;

        /**
         * @brief Create a new S3 object in the S3 object table
         *
         * @param object object entity
         * @return created object entity
         * @throws DatabaseException
         */
        Entity::S3::Object CreateObject(const Entity::S3::Object &object);

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param object object entity
         * @return updated object entity
         * @throws DatabaseException
         */
        Entity::S3::Object UpdateObject(const Entity::S3::Object &object);

        /**
         * @brief Gets an object from an bucket
         *
         * @param oid object ID
         * @return S3 object
         * @throws DatabaseException
         */
        Entity::S3::Object GetObjectById(const std::string &oid);

        /**
         * @brief Gets an object from an bucket
         *
         * @param region AWS S3 region name
         * @param bucket object bucket
         * @param key object key
         * @return S3 object
         * @throws DatabaseException
         */
        Entity::S3::Object GetObject(const std::string &region, const std::string &bucket, const std::string &key);

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
        Entity::S3::Object GetObjectMd5(const std::string &region, const std::string &bucket, const std::string &key, const std::string &md5sum);

        /**
         * @brief Counts the number of keys in a bucket
         *
         * @param region AWS region
         * @param bucket bucket name
         * @return number of object in bucket
         * @throws DatabaseException
         */
        long ObjectCount(const std::string &region = {}, const std::string &bucket = {}) const;

        /**
         * @brief Delete an object.
         *
         * @param object object entity
         * @throws DatabaseException
         */
        void DeleteObject(const Entity::S3::Object &object);

        /**
         * @brief Updates an existing object in the S3 object table
         *
         * @param bucket bucket to delete from
         * @param keys vector of object keys
         * @throws DatabaseException
         */
        void DeleteObjects(const std::string &bucket, const std::vector<std::string> &keys);

        /**
         * @brief Deletes all objects
         *
         * @retrun number of objects deleted.
         */
        long DeleteAllObjects();

      private:

        /**
         * S3 bucket map, when running without database
         */
        std::map<std::string, Entity::S3::Bucket> _buckets{};

        /**
         * S3 object map, when running without database
         */
        std::map<std::string, Entity::S3::Object> _objects{};

        /**
         * Bucket mutex
         */
        static boost::mutex _bucketMutex;

        /**
         * Object mutex
         */
        static boost::mutex _objectMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_S3_MEMORYDB_H
