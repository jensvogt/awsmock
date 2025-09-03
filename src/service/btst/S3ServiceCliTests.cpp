//
// Created by vogje01 on 02/06/2023.
//

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/s3/S3Server.h>

#define BOOST_TEST_MODULE S3ServiceCliTestss
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
    struct S3ServiceCliTests : TestBase {

        S3ServiceCliTests() {

            // General configuration
            StartGateway();

            _region = GetRegion();
            _endpoint = GetEndpoint();

            // Cleanup
            // ReSharper disable once CppExpressionWithoutSideEffects
            long deleted = _database.DeleteAllObjects();
            log_debug << "Objects deleted, count: " << deleted;
            // ReSharper disable once CppExpressionWithoutSideEffects
            deleted = _database.DeleteAllBuckets();
            log_debug << "Buckets deleted, count: " << deleted;
        }

        std::string _endpoint, _output, _region;
        Database::S3Database &_database = Database::S3Database::instance();
    };

    BOOST_FIXTURE_TEST_CASE(BucketCreateTest, S3ServiceCliTests) {

        // arrange

        // act
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        const Database::Entity::S3::BucketList bucketList = _database.ListBuckets();

        // assert
        BOOST_CHECK_EQUAL(1, bucketList.size());
    }

    BOOST_FIXTURE_TEST_CASE(BucketListTest, S3ServiceCliTests) {

        // arrange
        std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "ls", "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(output2.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output2, TEST_BUCKET_NAME), true);
    }

    BOOST_FIXTURE_TEST_CASE(BucketDeleteTest, S3ServiceCliTests) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});

        // act
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "rb", TEST_BUCKET, "--endpoint", _endpoint});
        const Database::Entity::S3::BucketList bucketList = _database.ListBuckets();

        // assert
        BOOST_CHECK_EQUAL(0, bucketList.size());
    }

    BOOST_FIXTURE_TEST_CASE(ObjectCreateTest, S3ServiceCliTests) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});

        // act
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "cp", filename, TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});
        const std::vector<Database::Entity::S3::Object> objectList = _database.ListBucket("test-bucket");

        // assert
        BOOST_CHECK_EQUAL(1, objectList.size());
    }

    BOOST_FIXTURE_TEST_CASE(ObjectGetTest, S3ServiceCliTests) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "cp", filename, TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "cp", TEST_BUCKET + "/" + filename, filename, "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(10, Core::FileUtils::FileSize(filename));
        BOOST_CHECK_EQUAL(Core::FileUtils::FileExists(filename), true);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectCopyTest, S3ServiceCliTests) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "cp", filename, TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "cp", TEST_BUCKET + "/" + filename, TEST_BUCKET + "/test/" + filename, "--endpoint", _endpoint});
        const std::vector<Database::Entity::S3::Object> objectList = _database.ListBucket(TEST_BUCKET_NAME);

        // assert
        BOOST_CHECK_EQUAL(2, objectList.size());
        BOOST_CHECK_EQUAL(objectList[0].key == "test/" + filename || objectList[0].key == filename, true);
        BOOST_CHECK_EQUAL(objectList[1].key == "test/" + filename || objectList[1].key == filename, true);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectMoveTest, S3ServiceCliTests) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        const std::string objectName = Core::StringUtils::Split(filename, '/')[2];
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "cp", filename, TEST_BUCKET + "/" + objectName, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mv", TEST_BUCKET + "/" + objectName, TEST_BUCKET + "/test/" + objectName, "--endpoint", _endpoint});
        const std::vector<Database::Entity::S3::Object> objectList = _database.ListBucket(TEST_BUCKET_NAME);

        // assert
        BOOST_CHECK_EQUAL(1, objectList.size());
        BOOST_CHECK_EQUAL(objectList[0].key, "test/" + objectName);
    }

    BOOST_FIXTURE_TEST_CASE(ObjectMultipartUploadTest, S3ServiceCliTests) {

        // arrange: Create bucket
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3api", "create-multipart-upload", "--bucket", "test-bucket", "--key", "multipart-upload.json", "--endpoint", _endpoint});

        // arrange: CreateMultipartUpload
        Dto::S3::CreateMultipartUploadResult s3Result;
        Dto::S3::CreateMultipartUploadResult::FromJson(output2);

        // act
        std::string filename = Core::FileUtils::CreateTempFile("json", 10 * 1024 * 1024);
        std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3api", "upload-part", "--bucket", "test-bucket", "--key", "multipart-upload.json", "--part-number", "1", "--body", filename, "--upload-id", s3Result.uploadId, "--endpoint", _endpoint});

        //aws s3api complete-multipart-upload --multipart-upload file://fileparts.json --bucket DOC-EXAMPLE-BUCKET --key large_BOOST_FIXTURE_TEST_CASEile --upload-userPoolId exampleTUVGeKAk3Ob7qMynRKqe3ROcavPRwg92eA6JPD4ybIGRxJx9R0VbgkrnOVphZFK59KCYJAO1PXlrBSW7vcH7ANHZwTTf0ovqe6XPYHwsSp7eTRnXB1qjx40Tk
        std::string output4 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3api", "complete-multipart-upload", "--bucket", "test-bucket", "--key", "multipart-upload.json", "--upload-id", s3Result.uploadId, "--endpoint", _endpoint});
        std::vector<Database::Entity::S3::Object> objectList = _database.ListBucket(TEST_BUCKET_NAME);

        // assert
        BOOST_CHECK_EQUAL(1, objectList.size());
        BOOST_CHECK_EQUAL(objectList[0].key, "multipart-upload.json");
    }

    BOOST_FIXTURE_TEST_CASE(ObjectDeleteTest, S3ServiceCliTests) {

        // arrange
        const std::string filename = Core::FileUtils::CreateTempFile("json", 10);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "mb", TEST_BUCKET, "--endpoint", _endpoint});
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "cp", filename, TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});

        // act
        std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"s3", "rm", TEST_BUCKET + "/" + filename, "--endpoint", _endpoint});
        const std::vector<Database::Entity::S3::Object> objectList = _database.ListBucket("test-bucket");

        // assert
        BOOST_CHECK_EQUAL(0, objectList.size());
    }

}// namespace AwsMock::Service
