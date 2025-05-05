//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_S3_SERVICE_TEST_H
#define AWMOCK_CORE_S3_SERVICE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/s3/S3Service.h>

// Test includes
#include <awsmock/core/TestUtils.h>

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
    class S3ServiceTest : public ::testing::Test {

      protected:

        void SetUp() override {
            testFile = Core::FileUtils::CreateTempFile("/tmp", "json", 10);
        }

        void TearDown() override {
            long count = _database.DeleteAllObjects();
            log_info << "Database objects deleted: " << count << std::endl;
            count = _database.DeleteAllBuckets();
            log_info << "Database buckets deleted: " << count << std::endl;
            Core::FileUtils::DeleteFile(testFile);
        }

        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        Database::S3Database _database = Database::S3Database();
        S3Service _service;
        std::string testFile;
    };

    TEST_F(S3ServiceTest, BucketCreateTest) {

        // arrange
        const Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};

        // act
        auto [location, arn] = _service.CreateBucket(request);

        // assert
        EXPECT_TRUE(location == "eu-central-1");
    }

    TEST_F(S3ServiceTest, BucketListTest) {

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
        EXPECT_TRUE(listResponse.contents.size() > 0);
        EXPECT_TRUE(listResponse.contents.front().key == KEY);
    }

    TEST_F(S3ServiceTest, BucketListAllTest) {

        // arrange
        const Dto::S3::CreateBucketRequest createRequest = {.region = REGION, .name = BUCKET, .owner = OWNER};
        auto [location, arn] = _service.CreateBucket(createRequest);

        // act
        const Dto::S3::ListAllBucketResponse response = _service.ListAllBuckets();

        // assert
        EXPECT_TRUE(response.total > 0);
        EXPECT_TRUE(response.bucketList.front().bucketName == BUCKET);
    }

    TEST_F(S3ServiceTest, BucketDeleteTest) {

        // arrange
        const Dto::S3::CreateBucketRequest createRequest = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse createResponse = _service.CreateBucket(createRequest);

        // act
        const Dto::S3::DeleteBucketRequest s3Request = {.region = REGION, .bucket = BUCKET};
        _service.DeleteBucket(s3Request);
        const Dto::S3::ListAllBucketResponse response = _service.ListAllBuckets();

        // assert
        EXPECT_EQ(0, response.bucketList.size());
    }

    TEST_F(S3ServiceTest, ObjectPutTest) {

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
        EXPECT_TRUE(putResponse.bucket == BUCKET);
        EXPECT_TRUE(putResponse.key == KEY);
    }

    TEST_F(S3ServiceTest, ObjectGetTest) {

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
        EXPECT_TRUE(getResponse.bucket == BUCKET);
        EXPECT_TRUE(getResponse.key == KEY);
    }

    TEST_F(S3ServiceTest, ObjectDeleteTest) {

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
        EXPECT_NO_THROW({ _service.DeleteObject(deleteRequest); });

        // assert
    }

}// namespace AwsMock::Service

#endif// AWMOCK_CORE_S3_SERVICE_TEST_H