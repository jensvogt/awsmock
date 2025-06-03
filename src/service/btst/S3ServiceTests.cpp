//
// Created by vogje01 on 02/06/2023.
//

// AwsMock includes

#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/s3/S3Service.h>

#define BOOST_TEST_MODULE S3ServiceTests
#define REGION "eu-central-1"
#define BUCKET "test-bucket"
#define KEY "testfile.json"
#define OWNER "test-owner"
#define LOCATION_CONSTRAINT "<CreateBucketConfiguration xmlns=\"http://s3.amazonaws.com/doc/2006-03-01/\"> \
                                <LocationConstraint>eu-central-1</LocationConstraint> \
                             </CreateBucketConfiguration>"

namespace AwsMock::Service {

    /**
     * @brief Test the S3 services of AwsMock.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct S3ServiceTest : TestBase {

        S3ServiceTest() {
            testFile = Core::FileUtils::CreateTempFile("/tmp", "json", 10);

            // Cleanup
            // ReSharper disable once CppExpressionWithoutSideEffects
            _database.DeleteAllObjects();
            // ReSharper disable once CppExpressionWithoutSideEffects
            _database.DeleteAllBuckets();
            Core::FileUtils::DeleteFile(testFile);
        }

        S3Service _service;
        std::string testFile;
        Database::S3Database _database = Database::S3Database();
    };

    BOOST_FIXTURE_TEST_CASE(BucketCreateTest, S3ServiceTest) {

        // arrange
        const Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};

        // act
        auto [location, arn] = _service.CreateBucket(request);

        // assert
        BOOST_CHECK_EQUAL(location, "eu-central-1");
    }

    BOOST_FIXTURE_TEST_CASE(BucketListTest, S3ServiceTest) {

        // arrange
        const Dto::S3::CreateBucketRequest createRequest = {.region = REGION, .name = BUCKET, .owner = OWNER};
        auto [location, arn] = _service.CreateBucket(createRequest);
        std::ifstream ifs(testFile);
        Dto::S3::PutObjectRequest putRequest;
        putRequest.region = REGION;
        putRequest.bucket = BUCKET;
        putRequest.key = KEY;
        Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);
        const Dto::S3::ListBucketRequest listRequest = {.region = REGION, .name = BUCKET};

        // act
        const Dto::S3::ListBucketResponse listResponse = _service.ListBucket(listRequest);

        // assert
        BOOST_CHECK_EQUAL(listResponse.contents.size() > 0, true);
        BOOST_CHECK_EQUAL(listResponse.contents.front().key, KEY);
    }

    BOOST_FIXTURE_TEST_CASE(BucketListAllTest, S3ServiceTest) {

        // arrange
        const Dto::S3::CreateBucketRequest createRequest = {.region = REGION, .name = BUCKET, .owner = OWNER};
        auto [location, arn] = _service.CreateBucket(createRequest);

        // act
        const Dto::S3::ListAllBucketResponse response = _service.ListAllBuckets();

        // assert
        BOOST_CHECK_EQUAL(response.total > 0, true);
        BOOST_CHECK_EQUAL(response.bucketList.front().bucketName, BUCKET);
    }

    BOOST_FIXTURE_TEST_CASE(BucketDeleteTest, S3ServiceTest) {

        // arrange
        const Dto::S3::CreateBucketRequest createRequest = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse createResponse = _service.CreateBucket(createRequest);

        // act
        const Dto::S3::DeleteBucketRequest s3Request = {.region = REGION, .bucket = BUCKET};
        _service.DeleteBucket(s3Request);
        const Dto::S3::ListAllBucketResponse response = _service.ListAllBuckets();

        // assert
        BOOST_CHECK_EQUAL(0, response.bucketList.size());
    }

    BOOST_FIXTURE_TEST_CASE(ObjectPutTest, S3ServiceTest) {

        // arrange
        Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse response = _service.CreateBucket(request);
        std::ifstream ifs(testFile);

        // act
        Dto::S3::PutObjectRequest putRequest;
        putRequest.region = REGION;
        putRequest.bucket = BUCKET;
        putRequest.key = KEY;
        Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);

        // assert
        BOOST_CHECK_EQUAL(putResponse.bucket, BUCKET);
        BOOST_CHECK_EQUAL(putResponse.key, KEY);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectGetTest, S3ServiceTest) {

        // arrange
        Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse response = _service.CreateBucket(request);
        std::ifstream ifs(testFile);
        Dto::S3::PutObjectRequest putRequest;
        putRequest.region = REGION;
        putRequest.bucket = BUCKET;
        putRequest.key = KEY;
        Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);

        // act
        Dto::S3::GetObjectRequest getRequest;
        getRequest.region = REGION;
        getRequest.bucket = BUCKET;
        getRequest.key = KEY;
        Dto::S3::GetObjectResponse getResponse = _service.GetObject(getRequest);

        // assert
        BOOST_CHECK_EQUAL(getResponse.bucket, BUCKET);
        BOOST_CHECK_EQUAL(getResponse.key, KEY);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteTest, S3ServiceTest) {

        // arrange
        Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse response = _service.CreateBucket(request);
        std::ifstream ifs(testFile);
        Dto::S3::PutObjectRequest putRequest;
        putRequest.region = REGION;
        putRequest.bucket = BUCKET;
        putRequest.key = KEY;
        Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);

        // act
        Dto::S3::DeleteObjectRequest deleteRequest = {.region = REGION, .bucket = BUCKET, .key = KEY};
        BOOST_CHECK_NO_THROW({ _service.DeleteObject(deleteRequest); });

        // assert
    }

}// namespace AwsMock::Service
