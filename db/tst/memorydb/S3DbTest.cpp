
//
// Created by vogje01 on 02/06/2023.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
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
#define TEST_LAMBDA_NAME "none"
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

    struct S3MemoryDbFixture {
        S3MemoryDbFixture() = default;

        ~S3MemoryDbFixture() {
            const long objectCount = RepositoryFactory::instance().s3Repository()->deleteAllObjects();
            log_debug << "Objects deleted " << objectCount;
            const long bucketCount = RepositoryFactory::instance().s3Repository()->deleteAllBuckets();
            log_debug << "Bucket deleted " << bucketCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(S3MemoryDbTests, S3MemoryDbFixture)

    BOOST_AUTO_TEST_CASE(CreateBucket) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);

        // act
        bucket = s3Database->createBucket(bucket);

        // assert
        BOOST_CHECK_EQUAL(false, bucket.arn.empty());
        BOOST_CHECK_EQUAL(false, bucket.oid.empty());
        BOOST_CHECK_EQUAL(false, bucket.name.empty());
    }

    BOOST_AUTO_TEST_CASE(CountBuckets) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        BOOST_CHECK_EQUAL(bucket.name, TEST_BUCKET_NAME);
        BOOST_CHECK_EQUAL(bucket.region, TEST_REGION);

        // act
        const long result = s3Database->bucketCount({}, {});

        // assert
        BOOST_CHECK_EQUAL(1, result);
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
        BOOST_CHECK_EQUAL(result2.size(), 1);
        BOOST_CHECK_EQUAL(false, object1.oid.empty());
        BOOST_CHECK_EQUAL(false, object2.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(BucketHasObjets) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);
        Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
        object2 = s3Database->createObject(object2);

        // act
        const bool result = s3Database->hasObjects(bucket);

        // assert
        BOOST_CHECK_EQUAL(result, true);
        BOOST_CHECK_EQUAL(false, object1.oid.empty());
        BOOST_CHECK_EQUAL(false, object2.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(BucketSize) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object1 = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object1 = s3Database->createObject(object1);
        Entity::S3::Object object2 = CreateDefaultObject(TEST_BUCKET_NAME, "test2/key2");
        object2 = s3Database->createObject(object2);

        // act
        const std::int64_t totalSize = s3Database->getBucketSize(TEST_REGION, TEST_BUCKET_NAME);

        // assert
        BOOST_CHECK_EQUAL(totalSize, 10);
        BOOST_CHECK_EQUAL(false, object1.oid.empty());
        BOOST_CHECK_EQUAL(false, object2.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(BucketDelete) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

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

        // act
        BOOST_CHECK_NO_THROW({ long deleted = s3Database->deleteAllBuckets(); });
        const bool result = s3Database->bucketExists(bucket.region, bucket.name);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_CASE(ObjectExists) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object = s3Database->createObject(object);

        // act
        const bool result = s3Database->objectExists(object);

        // assert
        BOOST_CHECK_EQUAL(result, true);
        BOOST_CHECK_EQUAL(false, bucket.oid.empty());
        BOOST_CHECK_EQUAL(false, object.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectCreate) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object = s3Database->createObject(object);

        // act
        const Entity::S3::Object result = s3Database->getObject(TEST_REGION, object.bucket, object.key);

        // assert
        BOOST_CHECK_EQUAL(result.key, object.key);
        BOOST_CHECK_EQUAL(false, bucket.oid.empty());
        BOOST_CHECK_EQUAL(false, object.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectUpdate) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object = s3Database->createObject(object);
        Entity::S3::Object updateObject;
        updateObject.bucket = bucket.name;
        updateObject.key = object.key;
        updateObject.owner = TEST_OWNER_NAME;
        updateObject.size = object.size + 10;

        // act
        const Entity::S3::Object result = s3Database->updateObject(updateObject);

        // assert
        BOOST_CHECK_EQUAL(15, result.size);
        BOOST_CHECK_EQUAL(false, bucket.oid.empty());
        BOOST_CHECK_EQUAL(false, object.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectById) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object = s3Database->createObject(object);

        // act
        const Entity::S3::Object result = s3Database->getObjectById(object.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, object.oid);
        BOOST_CHECK_EQUAL(false, bucket.oid.empty());
        BOOST_CHECK_EQUAL(false, object.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectDelete) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object = s3Database->createObject(object);

        // act
        BOOST_CHECK_NO_THROW({ long deleted = s3Database->deleteObject(object); });
        const bool result = s3Database->objectExists(object.region, object.bucket, object.key);

        // assert
        BOOST_CHECK_EQUAL(result, false);
        BOOST_CHECK_EQUAL(false, bucket.oid.empty());
        BOOST_CHECK_EQUAL(false, object.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ObjectBucketCount) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, std::string("test1/key-") + std::to_string(i));
            object = s3Database->createObject(object);
        }

        // act
        const long result = s3Database->objectCount(bucket.region, {}, {});

        // assert
        BOOST_CHECK_EQUAL(10, result);
    }

    BOOST_AUTO_TEST_CASE(ObjectList) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);

        // Create objects
        for (int i = 0; i < 10; i++) {
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, std::string("test1/key-") + std::to_string(i));
            object = s3Database->createObject(object);
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

        // Create objects
        std::vector<std::string> keys;
        for (int i = 0; i < 10; i++) {
            std::string key = std::string(TEST_OBJECT_KEY) + "-" + std::to_string(i);
            keys.push_back(key);
            Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, key);
            object = s3Database->createObject(object);
        }

        // act
        BOOST_CHECK_NO_THROW({ long deleted = s3Database->deleteObjects(TEST_REGION, bucket.name, keys); });
        const long result = s3Database->objectCount(TEST_REGION, bucket.name, {});

        // assert
        BOOST_CHECK_EQUAL(0, result);
    }

    BOOST_AUTO_TEST_CASE(ObjectDeleteAll) {

        // arrange
        const std::shared_ptr<IS3Repository> s3Database = RepositoryFactory::instance().s3Repository();
        Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_BUCKET_NAME);
        bucket = s3Database->createBucket(bucket);
        Entity::S3::Object object = CreateDefaultObject(TEST_BUCKET_NAME, "test1/key1");
        object = s3Database->createObject(object);

        // act
        BOOST_CHECK_NO_THROW({
            const long deleted = s3Database->deleteAllObjects();
            log_debug << "Deleted: " << deleted;
        });
        const bool result = s3Database->objectExists(TEST_REGION, TEST_BUCKET_NAME, object.key);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
