//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_S3_DATABASE_TEST_H
#define AWMOCK_CORE_S3_DATABASE_TEST_H

// C++ standard includes
#include <iostream>

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>

// MongoDB includes
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>

#define BUCKET "test-bucket"
#define OBJECT "test-object"
#define OWNER "test-owner"

namespace AwsMock::Database {

    struct S3DatabaseTest {

        S3DatabaseTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~S3DatabaseTest() {
            long count = _s3Database.DeleteAllBuckets();
            log_debug << "S3 buckets deleted, count: " << count;
            count = _s3Database.DeleteAllObjects();
            log_debug << "S3 objects deleted, count: " << count;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(true);
        S3Database _s3Database = S3Database();
    };

    BOOST_FIXTURE_TEST_CASE(BucketCreateTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucket(bucket);

        // assert
        BOOST_CHECK_EQUAL(result.name, BUCKET);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(BucketCountTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const long result = _s3Database.BucketCount();

        // assert
        BOOST_CHECK_EQUAL(1, result);
        BOOST_CHECK_EQUAL(bucket.name.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(BucketExistsTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const bool result = _s3Database.BucketExists(bucket);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(BucketByRegionNameTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::Bucket result = _s3Database.GetBucketByRegionName(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result.name, bucket.name);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(BucketGetByIdTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::Bucket result = _s3Database.GetBucketById(bucket.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, bucket.oid);
    }

    BOOST_FIXTURE_TEST_CASE(BucketListTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::BucketList result = _s3Database.ListBuckets();

        // assert
        BOOST_CHECK_EQUAL(result.size(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(BucketListObjectTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        _s3Database.CreateBucket(bucket);
        Entity::S3::Object object1;
        object1.bucket = bucket.name;
        object1.key = OBJECT;
        object1.owner = OWNER;
        object1.size = 5;
        _s3Database.CreateObject(object1);
        Entity::S3::Object object2;
        object2.bucket = bucket.name;
        object2.key = "test1/" + std::string(OBJECT);
        object2.owner = OWNER;
        object2.size = 5;
        _s3Database.CreateObject(object2);

        // act
        const std::vector<Entity::S3::Object> result1 = _s3Database.ListBucket(bucket.name);
        const std::vector<Entity::S3::Object> result2 = _s3Database.ListBucket(bucket.name, "test1");

        // assert
        BOOST_CHECK_EQUAL(result1.size(), 2);
        BOOST_CHECK_EQUAL(result1[0].key, OBJECT);
        BOOST_CHECK_EQUAL(result2.size(), 1);
        BOOST_CHECK_EQUAL(result2[0].key, std::string("test1/") + std::string(OBJECT));
    }

    BOOST_FIXTURE_TEST_CASE(BucketSizeTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        _s3Database.CreateBucket(bucket);
        Entity::S3::Object object1;
        object1.region = _region;
        object1.bucket = bucket.name;
        object1.key = OBJECT;
        object1.owner = OWNER;
        object1.size = 5;
        _s3Database.CreateObject(object1);
        Entity::S3::Object object2;
        object2.region = _region;
        object2.bucket = bucket.name;
        object2.key = "test1/" + std::string(OBJECT);
        object2.owner = OWNER;
        object2.size = 5;
        _s3Database.CreateObject(object2);

        // act
        const long totalSize = _s3Database.GetBucketSize(_region, BUCKET);

        // assert
        BOOST_CHECK_EQUAL(totalSize, 10);
    }

    BOOST_FIXTURE_TEST_CASE(BucketHasObjetsTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        _s3Database.CreateBucket(bucket);
        Entity::S3::Object object1;
        object1.region = _region;
        object1.bucket = bucket.name;
        object1.key = OBJECT;
        object1.owner = OWNER;
        object1.size = 5;
        _s3Database.CreateObject(object1);
        Entity::S3::Object object2;
        object2.region = _region;
        object2.bucket = bucket.name;
        object2.key = "test1/" + std::string(OBJECT);
        object2.owner = OWNER;
        object2.size = 5;
        _s3Database.CreateObject(object2);

        // act
        const bool result = _s3Database.HasObjects(bucket);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(BucketDeleteTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteBucket(bucket); });
        const bool result = _s3Database.BucketExists(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(BucketDeleteAllTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteAllBuckets(); });
        const bool result = _s3Database.BucketExists(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectExistsTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object;
        object.bucket = bucket.name;
        object.key = OBJECT;
        object.owner = OWNER;
        object.size = 5;
        object = _s3Database.CreateObject(object);

        // act
        const bool result = _s3Database.ObjectExists(object);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectCreateTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object;
        object.region = _region;
        object.bucket = bucket.name;
        object.key = OBJECT;
        object.owner = OWNER;
        object.size = 5;
        object = _s3Database.CreateObject(object);

        // act
        const Entity::S3::Object result = _s3Database.GetObject(_region, object.bucket, object.key);

        // assert
        BOOST_CHECK_EQUAL(result.key, object.key);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectUpdateTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object;
        object.region = _region;
        object.bucket = bucket.name;
        object.key = OBJECT;
        object.owner = OWNER;
        object.size = 5;
        object = _s3Database.CreateObject(object);
        Entity::S3::Object updateObject;
        updateObject.region = _region;
        updateObject.bucket = bucket.name;
        updateObject.key = OBJECT;
        updateObject.owner = OWNER;
        updateObject.size = object.size + 10;

        // act
        const Entity::S3::Object result = _s3Database.UpdateObject(updateObject);

        // assert
        BOOST_CHECK_EQUAL(15, result.size);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectByIdTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object;
        object.region = _region;
        object.bucket = bucket.name;
        object.key = OBJECT;
        object.owner = OWNER;
        object.size = 5;
        object = _s3Database.CreateObject(object);

        // act
        const Entity::S3::Object result = _s3Database.GetObjectById(object.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, object.oid);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object;
        object.bucket = bucket.name;
        object.key = OBJECT;
        object.owner = OWNER;
        object.size = 5;
        object = _s3Database.CreateObject(object);

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteObject(object); });
        const bool result = _s3Database.ObjectExists(object.region, object.bucket, object.key);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectBucketCountTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object;
            object.region = _region;
            object.bucket = bucket.name;
            object.key = std::string(OBJECT) + std::to_string(i);
            object.owner = OWNER;
            _s3Database.CreateObject(object);
        }

        // act
        const long result = _s3Database.ObjectCount(bucket.region, {}, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(10, result);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectListTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object;
            object.region = _region;
            object.bucket = bucket.name;
            object.key = std::string(OBJECT) + std::to_string(i);
            object.owner = OWNER;
            object = _s3Database.CreateObject(object);
        }

        // act
        const std::vector<Entity::S3::Object> result = _s3Database.ListObjects();

        // assert
        BOOST_CHECK_EQUAL(10, result.size());
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteManyTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);

        // Create objects
        std::vector<std::string> keys;
        for (int i = 0; i < 10; i++) {
            std::string key = std::string(OBJECT) + "-" + std::to_string(i);
            keys.push_back(key);
            Entity::S3::Object object;
            object.bucket = bucket.name;
            object.key = key;
            object.owner = OWNER;
            _s3Database.CreateObject(object);
        }

        // act
        BOOST_CHECK_NO_THROW({ _s3Database.DeleteObjects(_region, bucket.name, keys); });
        const bool result = _s3Database.ObjectCount(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(0, result);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteAllTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object;
        object.bucket = bucket.name;
        object.owner = OWNER;
        object.size = 5;
        object = _s3Database.CreateObject(object);

        // act
        BOOST_CHECK_NO_THROW({
            long deleted = _s3Database.DeleteAllObjects();
            log_debug << "Deleted: " << deleted;
        });
        const bool result = _s3Database.ObjectExists(object.region, object.bucket, object.key);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(CreateNotificationTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification;
        notification.event = "s3:ObjectCreated:*";
        notification.lambdaArn = "aws:arn:000000000:lambda:test";

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(4, result.notifications.size());
    }

    BOOST_FIXTURE_TEST_CASE(CreateNotificationPutTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification;
        notification.event = "s3:ObjectCreated:Put";
        notification.lambdaArn = "aws:arn:000000000:lambda:test";

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(1, result.notifications.size());
    }

    BOOST_FIXTURE_TEST_CASE(CreateNotificationTwiceTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification;
        notification.event = "s3:ObjectCreated:Put";
        notification.lambdaArn = "aws:arn:000000000:lambda:test";
        bucket = _s3Database.CreateBucketNotification(bucket, notification);

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(1, result.notifications.size());
    }

    BOOST_FIXTURE_TEST_CASE(DeleteNotificationTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification;
        notification.event = "s3:ObjectCreated:*";
        notification.lambdaArn = "aws:arn:000000000:lambda:test";
        bucket = _s3Database.CreateBucketNotification(bucket, notification);

        // act
        const Entity::S3::Bucket result = _s3Database.DeleteBucketNotifications(bucket, notification);

        // assert
        BOOST_CHECK_EQUAL(result.notifications.empty(), true);
    }

    BOOST_FIXTURE_TEST_CASE(DeleteNotificationPutTest, S3DatabaseTest) {

        // arrange
        Entity::S3::Bucket bucket;
        bucket.region = _region;
        bucket.name = BUCKET;
        bucket.owner = OWNER;
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification;
        notification.event = "s3:ObjectCreated:*";
        notification.lambdaArn = "aws:arn:000000000:lambda:test";
        bucket = _s3Database.CreateBucketNotification(bucket, notification);
        Entity::S3::BucketNotification deleteNotification;
        deleteNotification.event = "s3:ObjectCreated:Put";
        deleteNotification.lambdaArn = "aws:arn:000000000:lambda:test";

        // act
        const Entity::S3::Bucket result = _s3Database.DeleteBucketNotifications(bucket, deleteNotification);

        // assert
        BOOST_CHECK_EQUAL(0, result.notifications.size());
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_S3_DATABASE_TEST_H