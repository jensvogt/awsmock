//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_S3_MEMORYDB_TEST_H
#define AWMOCK_CORE_S3_MEMORYDB_TEST_H

// C++ standard includes
#include <vector>

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>

#define BUCKET "test-bucket"
#define OBJECT "test-object"
#define OWNER "test-owner"

namespace AwsMock::Database {

    struct S3MemoryDbTest {

        S3MemoryDbTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~S3MemoryDbTest() {
            long count = _s3Database.DeleteAllBuckets();
            log_debug << "S3 buckets deleted, count: " << count;
            count = _s3Database.DeleteAllObjects();
            log_debug << "S3 objects deleted, count: " << count;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(true);
        S3Database _s3Database = S3Database();
    };

    BOOST_FIXTURE_TEST_CASE(S3MemoryDbMTest, BucketCreateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucket(bucket);

        // assert
        BOOST_CHECK_EQUAL(result.name, BUCKET);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(BucketCountMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        BOOST_CHECK_EQUAL(bucket.name, BUCKET);
        BOOST_CHECK_EQUAL(bucket.region, _region);

        // act
        const long result = _s3Database.BucketCount();

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(BucketExistsMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const bool result = _s3Database.BucketExists(bucket);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(BucketByRegionNameMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::Bucket result = _s3Database.GetBucketByRegionName(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result.name, bucket.name);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(BucketGetByIdMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::Bucket result = _s3Database.GetBucketById(bucket.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, bucket.oid);
    }

    BOOST_FIXTURE_TEST_CASE(BucketListMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::BucketList result = _s3Database.ListBuckets();

        // assert
        BOOST_CHECK_EQUAL(result.size(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(BucketListObjectMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        _s3Database.CreateBucket(bucket);
        Entity::S3::Object object1 = {.bucket = bucket.name, .key = OBJECT, .owner = OWNER, .size = 5};
        _s3Database.CreateObject(object1);
        Entity::S3::Object object2 = {.bucket = bucket.name, .key = "test1/" + std::string(OBJECT), .owner = OWNER, .size = 5};
        _s3Database.CreateObject(object2);

        // act
        const Entity::S3::ObjectList result1 = _s3Database.ListBucket(bucket.name);
        const Entity::S3::ObjectList result2 = _s3Database.ListBucket(bucket.name, "test1");

        // assert
        BOOST_CHECK_EQUAL(result1.size(), 2);
        BOOST_CHECK_EQUAL(result2.size(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(BucketHasObjetsMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        _s3Database.CreateBucket(bucket);
        Entity::S3::Object object1 = {.region = _region, .bucket = bucket.name, .key = OBJECT, .owner = OWNER, .size = 5};
        _s3Database.CreateObject(object1);
        Entity::S3::Object
                object2 = {.region = _region, .bucket = bucket.name, .key = "test1/" + std::string(OBJECT), .owner = OWNER, .size = 5};
        _s3Database.CreateObject(object2);

        // act
        const bool result = _s3Database.HasObjects(bucket);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(BucketDeleteMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteBucket(bucket); });
        const bool result = _s3Database.BucketExists({.region = bucket.region, .name = bucket.name});

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(BucketDeleteAllMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteAllBuckets(); });
        const bool result = _s3Database.BucketExists({.region = bucket.region, .name = bucket.name});

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectExistsMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .key = OBJECT, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        const bool result = _s3Database.ObjectExists(object);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectCreateMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.region = _region, .bucket = bucket.name, .key = "TestKey", .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        const Entity::S3::Object result = _s3Database.GetObject(_region, object.bucket, object.key);

        // assert
        BOOST_CHECK_EQUAL(result.key, object.key);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectUpdateMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);
        Entity::S3::Object updateObject = {.bucket = bucket.name, .owner = OWNER, .size = object.size + 10};

        // act
        const Entity::S3::Object result = _s3Database.UpdateObject(updateObject);

        // assert
        BOOST_CHECK_EQUAL(15, result.size);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectByIdMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        const Entity::S3::Object result = _s3Database.GetObjectById(object.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, object.oid);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteObject(object); });
        const bool result = _s3Database.ObjectExists({.region = object.region, .bucket = object.bucket, .key = object.key});

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectBucketCountMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object = {.region = _region, .bucket = bucket.name, .key = std::string(OBJECT) + std::to_string(i), .owner = OWNER};
            object = _s3Database.CreateObject(object);
        }

        // act
        const long result = _s3Database.ObjectCount(bucket.region);

        // assert
        BOOST_CHECK_EQUAL(10, result);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectListMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object = {.region = _region, .bucket = bucket.name, .key = std::string(OBJECT) + std::to_string(i), .owner = OWNER};
            object = _s3Database.CreateObject(object);
        }

        // act
        const Entity::S3::ObjectList result = _s3Database.ListObjects();

        // assert
        BOOST_CHECK_EQUAL(10, result.size());
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteManyMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // Create objects
        std::vector<std::string> keys;
        for (int i = 0; i < 10; i++) {
            std::string key = std::string(OBJECT) + "-" + std::to_string(i);
            keys.push_back(key);
            Entity::S3::Object object = {.bucket = bucket.name, .key = key, .owner = OWNER};
            _s3Database.CreateObject(object);
        }

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteObjects(_region, bucket.arn, keys); });
        const bool result = _s3Database.ObjectCount(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(0, result);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteAllMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteAllObjects(); });
        const bool result = _s3Database.ObjectExists({.region = object.region, .bucket = object.bucket, .key = object.key});

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(CreateNotificationMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:*", .lambdaArn = "aws:arn:000000000:lambda:test"};

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(4, result.notifications.size());
    }

    BOOST_FIXTURE_TEST_CASE(CreateNotificationPutMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:Put", .lambdaArn = "aws:arn:000000000:lambda:test"};

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(1, result.notifications.size());
    }

    BOOST_FIXTURE_TEST_CASE(CreateNotificationTwiceMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:Put", .lambdaArn = "aws:arn:000000000:lambda:test"};
        bucket = _s3Database.CreateBucketNotification(bucket, notification);

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(1, result.notifications.size());
    }

    BOOST_FIXTURE_TEST_CASE(DeleteNotificationMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:*", .lambdaArn = "aws:arn:000000000:lambda:test"};
        bucket = _s3Database.CreateBucketNotification(bucket, notification);

        // act
        const Entity::S3::Bucket result = _s3Database.DeleteBucketNotifications(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(result.notifications.empty(), true);
    }

    BOOST_FIXTURE_TEST_CASE(DeleteNotificationPutMTest, S3MemoryDbTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:*", .lambdaArn = "aws:arn:000000000:lambda:test"};
        bucket = _s3Database.CreateBucketNotification(bucket, notification);
        const Entity::S3::BucketNotification deleteNotification = {.event = "s3:ObjectCreated:Put", .lambdaArn = "aws:arn:000000000:lambda:test"};

        // act
        const Entity::S3::Bucket result = _s3Database.DeleteBucketNotifications(bucket, deleteNotification);

        // assert
        BOOST_CHECK_EQUAL(0, result.notifications.size());
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_S3_MEMORYDB_TEST_H