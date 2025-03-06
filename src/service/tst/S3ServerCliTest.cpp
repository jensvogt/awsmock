//
// Created by vogje01 on 02/06/2023.
//

#include "TestBase.h"
#ifndef AWMOCK_SERVICE_S3_SERVER_CLI_TEST_H
#define AWMOCK_SERVICE_S3_SERVER_CLI_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/s3/S3Server.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define TEST_BUCKET_NAME std::string("test-bucket")
#define TEST_BUCKET std::string("s3://test-bucket")
#define S3_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    /**
     * @brief Test the S3 command line interface of AwsMock.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3ServerCliTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // General configuration
            StartGateway();

            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {
            _database.DeleteAllObjects();
            _database.DeleteAllBuckets();
        }

        std::string _endpoint, _output, _region;
        Database::S3Database &_database = Database::S3Database::instance();
    };

    TEST_F(S3ServerCliTest, BucketCreateTest) {

        // arrange

        // act
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        const Database::Entity::S3::BucketList bucketList = _database.ListBuckets();

        // assert
        EXPECT_EQ(1, bucketList.size());
    }

    TEST_F(S3ServerCliTest, BucketListTest) {

        // arrange
        std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});

        // act
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3", "ls", "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output2.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(output2, TEST_BUCKET_NAME));
    }

    TEST_F(S3ServerCliTest, BucketDeleteTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});

        // act
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3", "rb", TEST_BUCKET, "--endpoint", _endpoint});
        const Database::Entity::S3::BucketList bucketList = _database.ListBuckets();

        // assert
        EXPECT_EQ(0, bucketList.size());
    }

    TEST_F(S3ServerCliTest, ObjectCreateTest) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});

        // act
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3", "cp", filename, TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});
        const Database::Entity::S3::ObjectList objectList = _database.ListBucket("test-bucket");

        // assert
        EXPECT_EQ(1, objectList.size());
    }

    TEST_F(S3ServerCliTest, ObjectGetTest) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3", "cp", filename, TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand("aws", {"s3", "cp", TEST_BUCKET + "/" + filename + " " + filename, "--endpoint", _endpoint});

        // assert
        EXPECT_EQ(10, Core::FileUtils::FileSize(filename));
        EXPECT_TRUE(Core::FileUtils::FileExists(filename));
    }

    TEST_F(S3ServerCliTest, ObjectCopyTest) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3", "cp", filename + " " + TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand("aws", {"s3", "cp", TEST_BUCKET + "/" + filename, TEST_BUCKET + "/test/" + filename, "--endpoint", _endpoint});
        const Database::Entity::S3::ObjectList objectList = _database.ListBucket(TEST_BUCKET_NAME);

        // assert
        EXPECT_EQ(2, objectList.size());
        EXPECT_TRUE(objectList[0].key == "test/" + filename || objectList[0].key == filename);
        EXPECT_TRUE(objectList[1].key == "test/" + filename || objectList[1].key == filename);
    }

    TEST_F(S3ServerCliTest, ObjectMoveTest) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        const std::string objectName = Core::StringUtils::Split(filename, '/')[2];
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3", "cp", filename, TEST_BUCKET + "/" + objectName, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand("aws", {"s3", "mv", TEST_BUCKET + "/" + objectName, TEST_BUCKET + "/test/" + objectName, "--endpoint", _endpoint});
        const Database::Entity::S3::ObjectList objectList = _database.ListBucket(TEST_BUCKET_NAME);

        // assert
        EXPECT_EQ(1, objectList.size());
        EXPECT_TRUE(objectList[0].key == "test/" + objectName);
    }

    TEST_F(S3ServerCliTest, ObjectMultipartUploadTest) {

        // arrange: Create bucket
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3api", "create-multipart-upload", "--bucket", "test-bucket", "--key", "multipart-upload.json", "--endpoint " + _endpoint});

        // arrange: CreateMultipartUpload
        Dto::S3::CreateMultipartUploadResult s3Result;
        s3Result.FromJson(output2);

        // act
        std::string filename = Core::FileUtils::CreateTempFile("json", 10 * 1024 * 1024);
        std::string output3 = Core::TestUtils::SendCliCommand("aws", {"s3api", "upload-part", "--bucket", "test-bucket", "--key", "multipart-upload.json", "--part-number", "1", "--body", filename, "--upload-id", s3Result.uploadId, "--endpoint", _endpoint});

        //aws s3api complete-multipart-upload --multipart-upload file://fileparts.json --bucket DOC-EXAMPLE-BUCKET --key large_test_file --upload-userPoolId exampleTUVGeKAk3Ob7qMynRKqe3ROcavPRwg92eA6JPD4ybIGRxJx9R0VbgkrnOVphZFK59KCYJAO1PXlrBSW7vcH7ANHZwTTf0ovqe6XPYHwsSp7eTRnXB1qjx40Tk
        std::string output4 = Core::TestUtils::SendCliCommand("aws", {"s3api", "complete-multipart-upload", "--bucket", "test-bucket", "--key", "multipart-upload.json", "--upload-id", s3Result.uploadId, "--endpoint", _endpoint});
        Database::Entity::S3::ObjectList objectList = _database.ListBucket(TEST_BUCKET_NAME);

        // assert
        EXPECT_EQ(1, objectList.size());
        EXPECT_TRUE(objectList[0].key == "multipart-upload.json");
    }

    TEST_F(S3ServerCliTest, ObjectDeleteTest) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand("aws", {"s3", "cp", filename, TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand("aws", {"s3", "rm", TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});
        const Database::Entity::S3::ObjectList objectList = _database.ListBucket("test-bucket");

        // assert
        EXPECT_EQ(0, objectList.size());
    }
}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_S3_SERVER_CLI_TEST_H