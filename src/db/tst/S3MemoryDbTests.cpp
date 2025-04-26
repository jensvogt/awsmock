//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_S3_MEMORYDB_TEST_H
#define AWMOCK_CORE_S3_MEMORYDB_TEST_H

// C++ standard includes
#include <iostream>
#include <vector>

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>

// MongoDB includes
#include <bsoncxx/builder/basic/document.hpp>

#define BUCKET "test-bucket"
#define OBJECT "test-object"
#define OWNER "test-owner"

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    class S3MemoryDbTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.GetValue<std::string>("awsmock.region");
            Core::Configuration::instance().SetValue<bool>("awsmock.mongodb.active", false);
        }

        void TearDown() override {
            _s3Database.DeleteAllBuckets();
            _s3Database.DeleteAllObjects();
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        S3Database _s3Database = S3Database();
    };

    TEST_F(S3MemoryDbTest, BucketCreateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucket(bucket);

        // assert
        EXPECT_TRUE(result.name == BUCKET);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(S3MemoryDbTest, BucketCountTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        EXPECT_TRUE(bucket.name == BUCKET);
        EXPECT_TRUE(bucket.region == _region);

        // act
        const long result = _s3Database.BucketCount();

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(S3MemoryDbTest, BucketExistsTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const bool result = _s3Database.BucketExists(bucket);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3MemoryDbTest, BucketByRegionNameTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::Bucket result = _s3Database.GetBucketByRegionName(bucket.region, bucket.name);

        // assert
        EXPECT_TRUE(result.name == bucket.name);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(S3MemoryDbTest, BucketGetByIdTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::Bucket result = _s3Database.GetBucketById(bucket.oid);

        // assert
        EXPECT_EQ(result.oid, bucket.oid);
    }

    TEST_F(S3MemoryDbTest, BucketListTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        _s3Database.CreateBucket(bucket);

        // act
        const Entity::S3::BucketList result = _s3Database.ListBuckets();

        // assert
        EXPECT_EQ(result.size(), 1);
    }

    TEST_F(S3MemoryDbTest, BucketListObjectTest) {

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
        EXPECT_EQ(result1.size(), 2);
        EXPECT_EQ(result2.size(), 1);
    }

    TEST_F(S3MemoryDbTest, BucketHasObjetsTest) {

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
        EXPECT_TRUE(result);
    }

    TEST_F(S3MemoryDbTest, BucketDeleteTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        EXPECT_NO_THROW({ _s3Database.DeleteBucket(bucket); });
        const bool result = _s3Database.BucketExists({.region = bucket.region, .name = bucket.name});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3MemoryDbTest, BucketDeleteAllTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);

        // act
        EXPECT_NO_THROW({ _s3Database.DeleteAllBuckets(); });
        const bool result = _s3Database.BucketExists({.region = bucket.region, .name = bucket.name});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3MemoryDbTest, ObjectExistsTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .key = OBJECT, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        const bool result = _s3Database.ObjectExists(object);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(S3MemoryDbTest, ObjectCreateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.region = _region, .bucket = bucket.name, .key = "TestKey", .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        const Entity::S3::Object result = _s3Database.GetObject(_region, object.bucket, object.key);

        // assert
        EXPECT_TRUE(result.key == object.key);
    }

    TEST_F(S3MemoryDbTest, ObjectUpdateTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);
        Entity::S3::Object updateObject = {.bucket = bucket.name, .owner = OWNER, .size = object.size + 10};

        // act
        Entity::S3::Object result = _s3Database.UpdateObject(updateObject);

        // assert
        EXPECT_EQ(15, result.size);
    }

    TEST_F(S3MemoryDbTest, ObjectByIdTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        const Entity::S3::Object result = _s3Database.GetObjectById(object.oid);

        // assert
        EXPECT_TRUE(result.oid == object.oid);
    }

    TEST_F(S3MemoryDbTest, ObjectDeleteTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        EXPECT_NO_THROW({ _s3Database.DeleteObject(object); });
        const bool result = _s3Database.ObjectExists({.region = object.region, .bucket = object.bucket, .key = object.key});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3MemoryDbTest, ObjectBucketCountTest) {

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
        EXPECT_EQ(10, result);
    }

    TEST_F(S3MemoryDbTest, ObjectListTest) {

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
        EXPECT_EQ(10, result.size());
    }

    TEST_F(S3MemoryDbTest, ObjectDeleteManyTest) {

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
        EXPECT_NO_THROW({ _s3Database.DeleteObjects(bucket.name, keys); });
        const bool result = _s3Database.ObjectCount(bucket.region, bucket.name);

        // assert
        EXPECT_EQ(0, result);
    }

    TEST_F(S3MemoryDbTest, ObjectDeleteAllTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::Object object = {.bucket = bucket.name, .owner = OWNER, .size = 5};
        object = _s3Database.CreateObject(object);

        // act
        EXPECT_NO_THROW({ _s3Database.DeleteAllObjects(); });
        const bool result = _s3Database.ObjectExists({.region = object.region, .bucket = object.bucket, .key = object.key});

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(S3MemoryDbTest, CreateNotificationTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:*", .lambdaArn = "aws:arn:000000000:lambda:test"};

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        EXPECT_EQ(4, result.notifications.size());
    }

    TEST_F(S3MemoryDbTest, CreateNotificationPutTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:Put", .lambdaArn = "aws:arn:000000000:lambda:test"};

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        EXPECT_EQ(1, result.notifications.size());
    }

    TEST_F(S3MemoryDbTest, CreateNotificationTwiceTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:Put", .lambdaArn = "aws:arn:000000000:lambda:test"};
        bucket = _s3Database.CreateBucketNotification(bucket, notification);

        // act
        const Entity::S3::Bucket result = _s3Database.CreateBucketNotification(bucket, notification);

        // assert
        EXPECT_EQ(1, result.notifications.size());
    }

    TEST_F(S3MemoryDbTest, DeleteNotificationTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:*", .lambdaArn = "aws:arn:000000000:lambda:test"};
        bucket = _s3Database.CreateBucketNotification(bucket, notification);

        // act
        const Entity::S3::Bucket result = _s3Database.DeleteBucketNotifications(bucket, notification);

        // assert
        EXPECT_TRUE(result.notifications.empty());
    }

    TEST_F(S3MemoryDbTest, DeleteNotificationPutTest) {

        // arrange
        Entity::S3::Bucket bucket = {.region = _region, .name = BUCKET, .owner = OWNER};
        bucket = _s3Database.CreateBucket(bucket);
        const Entity::S3::BucketNotification notification = {.event = "s3:ObjectCreated:*", .lambdaArn = "aws:arn:000000000:lambda:test"};
        bucket = _s3Database.CreateBucketNotification(bucket, notification);
        const Entity::S3::BucketNotification deleteNotification = {.event = "s3:ObjectCreated:Put", .lambdaArn = "aws:arn:000000000:lambda:test"};

        // act
        const Entity::S3::Bucket result = _s3Database.DeleteBucketNotifications(bucket, deleteNotification);

        // assert
        EXPECT_EQ(3, result.notifications.size());
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_S3_MEMORYDB_TEST_H