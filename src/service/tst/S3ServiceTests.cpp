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
            _database.DeleteAllBuckets();
            Core::FileUtils::DeleteFile(testFile);
        }

        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        Database::S3Database _database = Database::S3Database();
        S3Service _service;
        std::string testFile;
    };

    TEST_F(S3ServiceTest, BucketCreateTest) {

        // arrange
        Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};

        // act
        Dto::S3::CreateBucketResponse response = _service.CreateBucket(request);

        // assert
        EXPECT_TRUE(response.location == "eu-central-1");
    }

    TEST_F(S3ServiceTest, BucketDeleteTest) {

        // arrange
        Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse response = _service.CreateBucket(request);

        // act
        Dto::S3::DeleteBucketRequest s3Request = {.region = REGION, .bucket = BUCKET};
        _service.DeleteBucket(s3Request);
        Dto::S3::ListAllBucketResponse result = _service.ListAllBuckets();

        // assert
        EXPECT_EQ(0, result.bucketList.size());
    }

    TEST_F(S3ServiceTest, ObjectPutTest) {

        // arrange
        Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse response = _service.CreateBucket(request);
        std::ifstream ifs(testFile);

        // act
        Dto::S3::PutObjectRequest putRequest = {.region = REGION, .bucket = BUCKET, .key = KEY};
        Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);

        // assert
        EXPECT_TRUE(putResponse.bucket == BUCKET);
        EXPECT_TRUE(putResponse.key == KEY);
    }

    TEST_F(S3ServiceTest, ObjectDeleteTest) {

        // arrange
        Dto::S3::CreateBucketRequest request = {.region = REGION, .name = BUCKET, .owner = OWNER};
        Dto::S3::CreateBucketResponse response = _service.CreateBucket(request);
        std::ifstream ifs(testFile);
        Dto::S3::PutObjectRequest putRequest = {.region = REGION, .bucket = BUCKET, .key = KEY};
        Dto::S3::PutObjectResponse putResponse = _service.PutObject(putRequest, ifs);

        // act
        Dto::S3::DeleteObjectRequest deleteRequest = {.region = REGION, .bucket = BUCKET, .key = KEY};
        EXPECT_NO_THROW({ _service.DeleteObject(deleteRequest); });

        // assert
    }

}// namespace AwsMock::Service

#endif// AWMOCK_CORE_S3_SERVICE_TEST_H