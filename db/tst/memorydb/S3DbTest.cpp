
//
// Created by vogje01 on 02/06/2023.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/S3Database.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_BUCKET_NAME "test-bucket"
#define TEST_OBJECT_KEY "test-key"
#define TEST_OWNER_NAME "none"
#define TEST_MESSAGE_BODY "{\"testAttribute\":\"testValue\"}"

namespace AwsMock::Database {

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

    struct S3MemoryDbFixture {
        S3MemoryDbFixture() = default;

        ~S3MemoryDbFixture() {
            const long objectCount = S3Database::instance().DeleteAllObjects();
            log_debug << "Objects deleted " << objectCount;
            const long bucketCount = S3Database::instance().DeleteAllBuckets();
            log_debug << "Bucket deleted " << bucketCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(S3MemoryDbTests, S3MemoryDbFixture)

        BOOST_AUTO_TEST_CASE(CreateBucket) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);

            // act
            bucket = s3Database.CreateBucket(bucket);

            // assert
            BOOST_CHECK_EQUAL(false, bucket.arn.empty());
            BOOST_CHECK_EQUAL(false, bucket.oid.empty());
            BOOST_CHECK_EQUAL(false, bucket.name.empty());
        }

        BOOST_AUTO_TEST_CASE(CountBuckets) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            bucket = s3Database.CreateBucket(bucket);
            BOOST_CHECK_EQUAL(bucket.name, TEST_BUCKET_NAME);
            BOOST_CHECK_EQUAL(bucket.region, TEST_REGION);

            // act
            const long result = s3Database.BucketCount();

            // assert
            BOOST_CHECK_EQUAL(1, result);
        }

        BOOST_AUTO_TEST_CASE(BucketExists) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            bucket = s3Database.CreateBucket(bucket);

            // act
            const bool result = s3Database.BucketExists(bucket);

            // assert
            BOOST_CHECK_EQUAL(result, true);
        }

        BOOST_AUTO_TEST_CASE(BucketByRegionName) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            bucket = s3Database.CreateBucket(bucket);

            // act
            const Entity::S3::Bucket result = s3Database.GetBucketByRegionName(bucket.region, bucket.name);

            // assert
            BOOST_CHECK_EQUAL(result.name, bucket.name);
            BOOST_CHECK_EQUAL(result.region, TEST_REGION);
        }

        BOOST_AUTO_TEST_CASE(BucketGetById) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            bucket = s3Database.CreateBucket(bucket);

            // act
            const Entity::S3::Bucket result = s3Database.GetBucketById(bucket.oid);

            // assert
            BOOST_CHECK_EQUAL(result.oid, bucket.oid);
        }

        BOOST_AUTO_TEST_CASE(BucketList) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);

            // act
            const Entity::S3::BucketList result = s3Database.ListBuckets();

            // assert
            BOOST_CHECK_EQUAL(result.size(), 1);
        }

        BOOST_AUTO_TEST_CASE(BucketListObject) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object1);
            Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
            s3Database.CreateObject(object2);

            // act
            const std::vector<Entity::S3::Object> result1 = s3Database.ListBucket(bucket.name);
            const std::vector<Entity::S3::Object> result2 = s3Database.ListBucket(bucket.name, "test1");

            // assert
            BOOST_CHECK_EQUAL(result1.size(), 2);
            BOOST_CHECK_EQUAL(result2.size(), 1);
        }

        BOOST_AUTO_TEST_CASE(BucketHasObjets) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object1);
            Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
            s3Database.CreateObject(object2);

            // act
            const bool result = s3Database.HasObjects(bucket);

            // assert
            BOOST_CHECK_EQUAL(result, true);
        }

        BOOST_AUTO_TEST_CASE(BucketSize) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            bucket = s3Database.CreateBucket(bucket);
            Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object1);
            Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
            s3Database.CreateObject(object2);

            // act
            const long totalSize = s3Database.GetBucketSize(TEST_REGION, TEST_BUCKET_NAME);

            // assert
            BOOST_CHECK_EQUAL(totalSize, 10);
        }

        BOOST_AUTO_TEST_CASE(BucketDelete) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);

            // act
            BOOST_CHECK_NO_THROW({ s3Database.DeleteBucket(bucket); });
            const bool result = s3Database.BucketExists(bucket.region, bucket.name);

            // assert
            BOOST_CHECK_EQUAL(result, false);
        }

        BOOST_AUTO_TEST_CASE(BucketDeleteAll) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);

            // act
            BOOST_CHECK_NO_THROW({ s3Database.DeleteAllBuckets(); });
            const bool result = s3Database.BucketExists(bucket.region, bucket.name);

            // assert
            BOOST_CHECK_EQUAL(result, false);
        }

        BOOST_AUTO_TEST_CASE(ObjectExists) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object);

            // act
            const bool result = s3Database.ObjectExists(object);

            // assert
            BOOST_CHECK_EQUAL(result, true);
        }

        BOOST_AUTO_TEST_CASE(ObjectCreate) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object);

            // act
            const Entity::S3::Object result = s3Database.GetObject(TEST_REGION, object.bucket, object.key);

            // assert
            BOOST_CHECK_EQUAL(result.key, object.key);
        }

        BOOST_AUTO_TEST_CASE(ObjectUpdate) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object);
            Entity::S3::Object updateObject;
            updateObject.bucket = bucket.name;
            updateObject.key = object.key;
            updateObject.owner = TEST_OWNER_NAME;
            updateObject.size = object.size + 10;

            // act
            const Entity::S3::Object result = s3Database.UpdateObject(updateObject);

            // assert
            BOOST_CHECK_EQUAL(15, result.size);
        }

        BOOST_AUTO_TEST_CASE(ObjectById) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object);

            // act
            const Entity::S3::Object result = s3Database.GetObjectById(object.oid);

            // assert
            BOOST_CHECK_EQUAL(result.oid, object.oid);
        }

        BOOST_AUTO_TEST_CASE(ObjectDelete) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object);

            // act
            BOOST_CHECK_NO_THROW({ s3Database.DeleteObject(object); });
            const bool result = s3Database.ObjectExists(object.region, object.bucket, object.key);

            // assert
            BOOST_CHECK_EQUAL(result, false);
        }

        BOOST_AUTO_TEST_CASE(ObjectBucketCount) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);

            // Create objects
            for (int i = 0; i < 10; i++) {
                Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, std::string("test1/key-") + std::to_string(i));
                s3Database.CreateObject(object);
            }

            // act
            const long result = s3Database.ObjectCount(bucket.region);

            // assert
            BOOST_CHECK_EQUAL(10, result);
        }

        BOOST_AUTO_TEST_CASE(ObjectList) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);

            // Create objects
            for (int i = 0; i < 10; i++) {
                Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, std::string("test1/key-") + std::to_string(i));
                s3Database.CreateObject(object);
            }

            // act
            const std::vector<Entity::S3::Object> result = s3Database.ListObjects();

            // assert
            BOOST_CHECK_EQUAL(10, result.size());
        }

        BOOST_AUTO_TEST_CASE(ObjectDeleteMany) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);

            // Create objects
            std::vector<std::string> keys;
            for (int i = 0; i < 10; i++) {
                std::string key = std::string(TEST_OBJECT_KEY) + "-" + std::to_string(i);
                keys.push_back(key);
                Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, key);
                s3Database.CreateObject(object);
            }

            // act
            BOOST_CHECK_NO_THROW({ s3Database.DeleteObjects(TEST_REGION, bucket.name, keys); });
            const long result = s3Database.ObjectCount(TEST_REGION, bucket.name);

            // assert
            BOOST_CHECK_EQUAL(0, result);
        }

        BOOST_AUTO_TEST_CASE(ObjectDeleteAll) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
            s3Database.CreateObject(object);

            // act
            BOOST_CHECK_NO_THROW({
                const long deleted = s3Database.DeleteAllObjects();
                log_debug << "Deleted: " << deleted;
                });
            const bool result = s3Database.ObjectExists(TEST_REGION, TEST_BUCKET_NAME, object.key);

            // assert
            BOOST_CHECK_EQUAL(result, false);
        }

        // BOOST_AUTO_TEST_CASE(CreateNotification) {
        //
        //     // arrange
        //     const S3Database &s3Database = S3Database::instance();
        //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        //     s3Database.CreateBucket(bucket);
        //     Entity::S3::BucketNotification notification;
        //     notification.event = "s3:ObjectCreated:*";
        //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
        //
        //     // act
        //     const Entity::S3::Bucket result = s3Database.CreateBucketNotification(bucket, notification);
        //
        //     // assert
        //     BOOST_CHECK_EQUAL(4, result.notifications.size());
        // }

        // BOOST_AUTO_TEST_CASE(CreateNotificationPut) {
        //
        //     // arrange
        //     const S3Database &s3Database = S3Database::instance();
        //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        //     s3Database.CreateBucket(bucket);
        //     Entity::S3::BucketNotification notification;
        //     notification.event = "s3:ObjectCreated:Put";
        //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
        //
        //     // act
        //     const Entity::S3::Bucket result = s3Database.CreateBucketNotification(bucket, notification);
        //
        //     // assert
        //     BOOST_CHECK_EQUAL(1, result.notifications.size());
        // }

        // BOOST_AUTO_TEST_CASE(CreateNotificationTwice) {
        //
        //     // arrange
        //     const S3Database &s3Database = S3Database::instance();
        //     Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        //     s3Database.CreateBucket(bucket);
        //     Entity::S3::BucketNotification notification;
        //     notification.event = "s3:ObjectCreated:Put";
        //     notification.lambdaArn = "aws:arn:000000000:lambda:test";
        //     bucket = s3Database.CreateBucketNotification(bucket, notification);
        //
        //     // act
        //     const Entity::S3::Bucket result = s3Database.CreateBucketNotification(bucket, notification);
        //
        //     // assert
        //     BOOST_CHECK_EQUAL(1, result.notifications.size());
        // }

        BOOST_AUTO_TEST_CASE(DeleteNotification) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::BucketNotification notification;
            notification.event = "s3:ObjectCreated:*";
            notification.lambdaArn = "aws:arn:000000000:lambda:test";
            bucket = s3Database.CreateBucketNotification(bucket, notification);

            // act
            const Entity::S3::Bucket result = s3Database.DeleteBucketNotifications(bucket, notification);

            // assert
            BOOST_CHECK_EQUAL(result.notifications.empty(), true);
        }

        BOOST_AUTO_TEST_CASE(DeleteNotificationPut) {

            // arrange
            const S3Database &s3Database = S3Database::instance();
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
            s3Database.CreateBucket(bucket);
            Entity::S3::BucketNotification notification;
            notification.event = "s3:ObjectCreated:*";
            notification.lambdaArn = "aws:arn:000000000:lambda:test";
            bucket = s3Database.CreateBucketNotification(bucket, notification);
            Entity::S3::BucketNotification deleteNotification;
            deleteNotification.event = "s3:ObjectCreated:Put";
            deleteNotification.lambdaArn = "aws:arn:000000000:lambda:test";

            // act
            const Entity::S3::Bucket result = s3Database.DeleteBucketNotifications(bucket, deleteNotification);

            // assert
            BOOST_CHECK_EQUAL(3, result.notifications.size());
        }

    BOOST_AUTO_TEST_SUITE_END()

} // namespace AwsMock::Database
