//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_LAMBDA_LAMBDASERVERCLITEST_H
#define AWMOCK_LAMBDA_LAMBDASERVERCLITEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "TestBase.h"


#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/lambda/LambdaServer.h>
#include <awsmock/service/lambda/LambdaService.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define ACCOUNT_ID "000000000000"
#define FUNCTION_NAME std::string("test-function")
#define FUNCTION_FILE std::string("/tmp/java-basic-1.0-SNAPSHOT.jar")
#define ROLE std::string("arn:aws:iam::000000000000:role/lambda-role")
#define CODE std::string("S3Bucket=lambdaBucket,S3Key=lambdaKey")
#define RUNTIME std::string("java11")

namespace AwsMock::Service {

    /**
     * @brief AwsMock lambda integration test.
     *
     * @par
     * To run the test, the AWS test lambdas should be installed in /usr/local/lib. See https://github.com/awsdocs/aws-lambda-developer-guide/tree/main/sample-apps/java-basic for details.
     */
    class LambdaServerCliTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // Check lambda examples
            if (!Core::FileUtils::FileExists(FUNCTION_FILE)) {
                SUCCEED();
                return;
            }

            // Start the gateway server
            StartGateway();

            // Create Lambda bucket
            Dto::S3::CreateBucketRequest request;
            request.region = REGION;
            request.owner = OWNER;
            request.name = "lambdaBucket";
            Dto::S3::CreateBucketResponse response = _s3Service.CreateBucket(request);

            // General configuration
            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {

            try {

                // Delete lambda function
                Dto::Lambda::DeleteFunctionRequest deleteFunctionRequest = {.functionName = FUNCTION_NAME, .qualifier = "latest"};
                _lambdaService.DeleteFunction({.region = REGION, .functionName = FUNCTION_NAME, .qualifier = "latest"});

                // Delete all S3 objects
                long count = _s3Database.DeleteAllObjects();
                log_debug << "S3 objects deleted, count: " << count;
                count = _s3Database.DeleteAllBuckets();
                log_debug << "S3 buckets deleted, count: " << count;

            } catch (Core::ServiceException &ex) {
                // Do nothing
            }
        }

        std::string _endpoint, _region;
        boost::asio::io_context _ios{10};
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();
        Database::S3Database &_s3Database = Database::S3Database::instance();
        LambdaService _lambdaService;
        S3Service _s3Service;
        std::shared_ptr<GatewayServer> _gatewayServer;
    };

    /*
    TEST_F(LambdaServerCliTest, LambdaCreateTest) {

        // check lambda examples
        if (!Core::FileUtils::FileExists(FUNCTION_FILE)) {
            std::cerr << "Lambda test function missing" << std::endl;
            SUCCEED();
            return;
        }

        // arrange
        std::ifstream input_file(FUNCTION_FILE, std::ios::binary);
        Dto::S3::PutObjectRequest request;
        request.region = REGION;
        request.bucket = "lambdaBucket";
        request.key = "lambdaKey";
        _s3Service.PutObject(request, input_file);

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"lambda", "create-function", "--function-name", FUNCTION_NAME, "--role", ROLE, "--code", CODE, "--runtime", RUNTIME, "--endpoint", _endpoint});
        Database::Entity::Lambda::LambdaList lambdaList = _lambdaDatabase.ListLambdas();

        // assert
        EXPECT_EQ(1, lambdaList.size());
    }*/

    /*TEST_F(LambdaServerCliTest, LambdaListTest) {

        // check lambda examples
        if (!Core::FileUtils::FileExists(FUNCTION_FILE)) {
            std::cerr << "Lambda test function missing" << std::endl;
            SUCCEED();
            return;
        }

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws lambda create-function --function-name " + FUNCTION_NAME + " --role " + ROLE + " --code " + CODE + " --runtime " + RUNTIME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);

        // act
        Core::ExecResult listResult = Core::TestUtils::SendCliCommand("aws lambda list-functions --endpoint " + _endpoint);

        // assert
        EXPECT_EQ(0, listResult.status);
        EXPECT_FALSE(listResult.output.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(listResult.output, FUNCTION_NAME));
    }*/

    /*TEST_F(LambdaServerCliTest, BucketDeleteTest) {

    // arrange
    Core::ExecResult createResult = Core::TestUtils::SendCliCommand("sqs", "aws s3 mb " + TEST_BUCKET + " --endpoint " + _endpoint);
    EXPECT_EQ(0, createResult.status);

    // act
    Core::ExecResult result = Core::TestUtils::SendCliCommand("sqs", "aws s3 rb " + TEST_BUCKET + " --endpoint " + _endpoint);
    Database::Entity::S3::BucketList bucketList = _database.ListBuckets();

    // assert
    EXPECT_EQ(0, result.status);
    EXPECT_EQ(0, bucketList.size());
  }*/

}// namespace AwsMock::Service

#endif// AWMOCK_LAMBDA_LAMBDASERVERCLITEST_H