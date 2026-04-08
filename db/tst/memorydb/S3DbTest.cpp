
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
#define TEST_MESSAGE_BODY "{\"testAttribute\":\"testValue\"}"

namespace AwsMock::Database {

    Entity::S3::Bucket CreateDefaultBucket(const std::string &name) {
        Entity::S3::Bucket bucket;
        bucket.region = TEST_REGION;
        bucket.name = name;
        bucket.arn = Core::AwsUtils::CreateS3BucketArn(TEST_REGION,TEST_ACCOUNT_ID, name);
        return bucket;
    }

    Entity::S3::Object CreateDefaultObject(const std::string &region, const std::string bucketName) {
        Entity::S3::Object object;
        object.region = region;
        object.bucket = bucketName;
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
            Entity::S3::Bucket bucket = CreateDefaultBucket(TEST_REGION);

            // act
            bucket = s3Database.CreateBucket(bucket);

            // assert
            BOOST_CHECK_EQUAL(false, bucket.arn.empty());
            BOOST_CHECK_EQUAL(false, bucket.oid.empty());
            BOOST_CHECK_EQUAL(false, bucket.name.empty());
        }
    
    BOOST_AUTO_TEST_SUITE_END()

} // namespace AwsMock::Database
