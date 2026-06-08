//
// Created by vogje01 on 02/06/2023.
//

// C++ standard includes
#include <iostream>

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// Awsmock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/s3/IS3Repository.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_BUCKET_NAME "test-bucket"
#define TEST_OBJECT_KEY "test-key"
#define TEST_OWNER_NAME "none"
#define TEST_MESSAGE_BODY "{\"testAttribute\":\"testValue\"}"

namespace Awsmock::Database {

    Entity::S3::Bucket CreateDefaultBucket(const std::string &name) {
        Entity::S3::Bucket bucket;
        bucket.region = TEST_REGION;
        bucket.name = name;
        bucket.arn = Core::AwsUtils::CreateS3BucketArn(TEST_REGION, TEST_ACCOUNT_ID, name);
        return bucket;
    }

    Entity::S3::Object CreateDefaultObject(const std::string &bucketName, const std::string &key) {
        Entity::S3::Object object;
        object.region = TEST_REGION;
        object.bucket = bucketName;
        object.key = key;
        object.size = 5;
        return object;
    }

    struct S3DbFixture {
        S3DbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB, "test");
        }

        ~S3DbFixture() {
            const long objectCount = RepositoryFactory::instance().s3Repository()->deleteAllObjects();
            log_debug << "Objects deleted " << objectCount;
            const long bucketCount = RepositoryFactory::instance().s3Repository()->deleteAllBuckets();
            log_debug << "Bucket deleted " << bucketCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(S3DbTests, S3DbFixture)

    BOOST_AUTO_TEST_CASE(BucketCreate) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);

        // act
        const Entity::S3::Bucket result = s3Database->createBucket(bucket);

        // assert
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        BOOST_CHECK_EQUAL(false, bucket.oid.empty());
        BOOST_CHECK_EQUAL(false, bucket.name.empty());
    }

    BOOST_AUTO_TEST_CASE(BucketCount) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

        // act
        const long result = s3Database->bucketCount({}, {});

        // assert
        BOOST_CHECK_EQUAL(1, result);
        BOOST_CHECK_EQUAL(bucket.name.empty(), false);
    }

    BOOST_AUTO_TEST_CASE(BucketExists) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

        // act
        const bool result = s3Database->bucketExists(bucket);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(BucketByRegionName) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

        // act
        const Entity::S3::Bucket result = s3Database->getBucketByRegionName(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result.name, bucket.name);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
    }

    BOOST_AUTO_TEST_CASE(BucketGetById) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

        // act
        const Entity::S3::Bucket result = s3Database->getBucketById(bucket.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, bucket.oid);
    }

    BOOST_AUTO_TEST_CASE(BucketList) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

        // act
        const Entity::S3::BucketList result = s3Database->listBuckets({}, {}, 0, 0, {});

        // assert
        BOOST_CHECK_EQUAL(result.size(), 1);
    }

    BOOST_AUTO_TEST_CASE(BucketListObject) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);
        Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
        object2 = s3Database->createObject(object2);

        // act
        const std::vector<Entity::S3::Object> result1 = s3Database->listBucket(bucket.name, {});
        const std::vector<Entity::S3::Object> result2 = s3Database->listBucket(bucket.name, "test1");

        // assert
        BOOST_CHECK_EQUAL(result1.size(), 2);
        BOOST_CHECK_EQUAL(result1[0].key, "test1/key1");
        BOOST_CHECK_EQUAL(result1[1].key, "test2/key2");
        BOOST_CHECK_EQUAL(result2.size(), 1);
        BOOST_CHECK_EQUAL(result2[0].key, std::string("test1/key1"));
    }

    BOOST_AUTO_TEST_CASE(BucketSize) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);
        Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
        object2 = s3Database->createObject(object2);

        // act
        const long totalSize = s3Database->getBucketSize(TEST_REGION, TEST_BUCKET_NAME);

        // assert
        BOOST_CHECK_EQUAL(totalSize, 10);
    }

    BOOST_AUTO_TEST_CASE(BucketHasObjets) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);
        Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
        object2 = s3Database->createObject(object2);

        // act
        const bool result = s3Database->hasObjects(bucket);

        // assert
        BOOST_CHECK_EQUAL(result, true);
        BOOST_CHECK_EQUAL(false, object1.bucket.empty());
        BOOST_CHECK_EQUAL(false, object2.bucket.empty());
    }

    BOOST_AUTO_TEST_CASE(BucketDelete) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());

        // act
        BOOST_CHECK_NO_THROW({ s3Database->deleteBucket(bucket); });
        const bool result = s3Database->bucketExists(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_CASE(BucketDeleteAll) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());

        // act
        BOOST_CHECK_NO_THROW({
            const long deleted = s3Database->deleteAllBuckets();
            BOOST_CHECK_EQUAL(1, deleted);
        });
        const bool result = s3Database->bucketExists(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_CASE(ObjectExists) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);

        // act
        const bool result = s3Database->objectExists(object1);

        // assert
        BOOST_CHECK_EQUAL(result, true);
        BOOST_CHECK_EQUAL(false, object1.bucket.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectCreate) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);

        // act
        const Entity::S3::Object result = s3Database->getObject(TEST_REGION, object1.bucket, object1.key);

        // assert
        BOOST_CHECK_EQUAL(result.key, object1.key);
        BOOST_CHECK_EQUAL(false, object1.bucket.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectUpdate) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);
        Entity::S3::Object updateObject;
        updateObject.region = TEST_REGION;
        updateObject.bucket = bucket.name;
        updateObject.key = "test1/key1";
        updateObject.owner = TEST_OWNER_NAME;
        updateObject.size = object1.size + 10;

        // act
        const Entity::S3::Object result = s3Database->updateObject(updateObject);

        // assert
        BOOST_CHECK_EQUAL(15, result.size);
        BOOST_CHECK_EQUAL(false, object1.bucket.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectById) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);

        // act
        const Entity::S3::Object result = s3Database->getObjectById(object1.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, object1.oid);
    }

    BOOST_AUTO_TEST_CASE(ObjectDelete) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);

        // act
        BOOST_CHECK_NO_THROW({
            long deleted = s3Database->deleteObject(object1);
            BOOST_CHECK_EQUAL(1, deleted);
        });
        const bool result = s3Database->objectExists(object1.region, object1.bucket, object1.key);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_CASE(ObjectBucketCount) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key-" + std::to_string(i));
            object1 = s3Database->createObject(object1);
        }

        // act
        const long result = s3Database->objectCount(bucket.region, {}, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(10, result);
    }

    BOOST_AUTO_TEST_CASE(ObjectList) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key-" + std::to_string(i));
            object1 = s3Database->createObject(object1);
        }

        // act
        const std::vector<Entity::S3::Object> result = s3Database->listObjects({}, {}, {}, 0, 0, {});

        // assert
        BOOST_CHECK_EQUAL(10, result.size());
    }

    BOOST_AUTO_TEST_CASE(ObjectDeleteMany) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());

        // Create objects
        std::vector<std::string> keys;
        for (int i = 0; i < 10; i++) {
            std::string key = std::string("test1/key-") + std::to_string(i);
            keys.push_back(key);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, key);
            object = s3Database->createObject(object);
        }

        // act
        BOOST_CHECK_NO_THROW({
            const long deleted = s3Database->deleteObjects(TEST_REGION, bucket.name, keys);
            BOOST_CHECK_EQUAL(10, deleted);
        });
        const bool result = s3Database->objectCount(bucket.region, bucket.name, {});

        // assert
        BOOST_CHECK_EQUAL(0, result);
    }

    BOOST_AUTO_TEST_CASE(ObjectDeleteAll) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object = s3Database->createObject(object);

        // act
        BOOST_CHECK_NO_THROW({
            const long deleted = s3Database->deleteAllObjects();
            log_debug << "Deleted: " << deleted;
        });
        const bool result = s3Database->objectExists(object.region, object.bucket, object.key);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    //
    // BOOST_AUTO_TEST_CASE(CreateNotification) {
    //
    //     // arrange
    //     const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
    //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
    //     bucket = s3Database->createBucket(bucket);
    //     BOOST_CHECK_EQUAL(false, bucket.arn.empty());
    //     Entity::S3::BucketNotification notification;
    //     notification.event = "s3:ObjectCreated:*";
    //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
    //
    //     // act
    //     const Entity::S3::Bucket result = s3Database->createBucketNotification(bucket, notification);
    //
    //     // assert
    //     BOOST_CHECK_EQUAL(4, result.notifications.size());
    // }
    //
    // BOOST_AUTO_TEST_CASE(CreateNotificationPut) {
    //
    //     // arrange
    //     const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
    //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
    //     bucket = s3Database->createBucket(bucket);
    //     BOOST_CHECK_EQUAL(false, bucket.arn.empty());
    //     Entity::S3::BucketNotification notification;
    //     notification.event = "s3:ObjectCreated:Put";
    //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
    //
    //     // act
    //     const Entity::S3::Bucket result = s3Database->createBucketNotification(bucket, notification);
    //
    //     // assert
    //     BOOST_CHECK_EQUAL(1, result.notifications.size());
    // }
    //
    // BOOST_AUTO_TEST_CASE(CreateNotificationTwice) {
    //
    //     // arrange
    //     const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
    //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
    //     bucket = s3Database->createBucket(bucket);
    //     BOOST_CHECK_EQUAL(false, bucket.arn.empty());
    //     Entity::S3::BucketNotification notification;
    //     notification.event = "s3:ObjectCreated:Put";
    //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
    //     bucket = s3Database->createBucketNotification(bucket, notification);
    //
    //     // act
    //     const Entity::S3::Bucket result = s3Database->createBucketNotification(bucket, notification);
    //
    //     // assert
    //     BOOST_CHECK_EQUAL(1, result.notifications.size());
    // }
    //
    // BOOST_AUTO_TEST_CASE(DeleteNotification) {
    //
    //     // arrange
    //     const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
    //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
    //     bucket = s3Database->createBucket(bucket);
    //     BOOST_CHECK_EQUAL(false, bucket.arn.empty());
    //     Entity::S3::BucketNotification notification;
    //     notification.event = "s3:ObjectCreated:*";
    //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
    //     bucket = s3Database->createBucketNotification(bucket, notification);
    //
    //     // act
    //     const Entity::S3::Bucket result = s3Database->deleteBucketNotifications(bucket, notification);
    //
    //     // assert
    //     BOOST_CHECK_EQUAL(result.notifications.empty(), true);
    // }
    //
    // BOOST_AUTO_TEST_CASE(DeleteNotificationPut) {
    //
    //     // arrange
    //     const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
    //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
    //     bucket = s3Database->createBucket(bucket);
    //     BOOST_CHECK_EQUAL(false, bucket.arn.empty());
    //     Entity::S3::BucketNotification notification;
    //     notification.event = "s3:ObjectCreated:*";
    //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
    //     bucket = s3Database->createBucketNotification(bucket, notification);
    //     Entity::S3::BucketNotification deleteNotification;
    //     deleteNotification.event = "s3:ObjectCreated:Put";
    //     deleteNotification.lambdaArn = "aws:arn:000000000:lambda:test";
    //
    //     // act
    //     const Entity::S3::Bucket result = s3Database->deleteBucketNotifications(bucket, deleteNotification);
    //
    //     // assert
    //     BOOST_CHECK_EQUAL(0, result.notifications.size());
    // }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
