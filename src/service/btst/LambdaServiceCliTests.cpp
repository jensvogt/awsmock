//
// Created by vogje01 on 02/06/2023.
//

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
    struct LambdaServiceCliTests : TestBase {

        LambdaServiceCliTests() {

            // Check lambda examples
            if (!Core::FileUtils::FileExists(FUNCTION_FILE)) {
                BOOST_FAIL("Missing lambda function!");
                return;
            }

            // Start the gateway server
            StartGateway();

            // General configuration
            _region = GetRegion();
            _endpoint = GetEndpoint();

            // Cleanup
            try {

                // Delete all S3 objects
                // ReSharper disable once CppExpressionWithoutSideEffects
                _s3Database.DeleteAllObjects();
                // ReSharper disable once CppExpressionWithoutSideEffects
                _s3Database.DeleteAllBuckets();
                // ReSharper disable once CppExpressionWithoutSideEffects
                _lambdaDatabase.DeleteAllLambdas();

            } catch (Core::ServiceException &ex) {
                // Do nothing
            } catch (Core::DatabaseException &ex) {
                // Do nothing
            }

            // Create Lambda bucket
            Dto::S3::CreateBucketRequest request;
            request.region = REGION;
            request.owner = OWNER;
            request.name = "lambdaBucket";
            Dto::S3::CreateBucketResponse response = _s3Service.CreateBucket(request);

            // arrange
            std::ifstream input_file(FUNCTION_FILE, std::ios::binary);
            Dto::S3::PutObjectRequest putRequest;
            putRequest.region = REGION;
            putRequest.bucket = "lambdaBucket";
            putRequest.key = "lambdaKey";
            _s3Service.PutObject(putRequest, input_file);
        }

        boost::asio::io_context _ioContext;
        std::string _endpoint, _region;
        S3Service _s3Service{_ioContext};
        LambdaService _lambdaService{_ioContext};
        Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();
        Database::S3Database &_s3Database = Database::S3Database::instance();
    };


    BOOST_FIXTURE_TEST_CASE(LambdaCreateTest, LambdaServiceCliTests) {

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"lambda", "create-function", "--function-name", FUNCTION_NAME, "--role", ROLE, "--code", CODE, "--runtime", RUNTIME, "--endpoint", _endpoint});
        const Database::Entity::Lambda::LambdaList lambdaList = _lambdaDatabase.ListLambdas();

        // assert
        BOOST_CHECK_EQUAL(1, lambdaList.size());
    }

    /*
    BOOST_FIXTURE_TEST_CASE(LambdaListTest, LambdaServiceCliTests) {

        // arrange
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"lambda", "create-function", "--function-name", FUNCTION_NAME, "--role", ROLE, "--code", CODE, "--runtime", RUNTIME, "--endpoint", _endpoint});

        // act
        const std::string listResult = Core::TestUtils::SendCliCommand(AWS_CMD, {"lambda", "list-functions", "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(listResult, FUNCTION_NAME), true);
    }*/

    /*BOOST_FIXTURE_TEST_CASE(BucketDeleteTest,LambdaServiceCliTests) {

    // arrange
    Core::ExecResult createResult = Core::TestUtils::SendCliCommand("sqs", "aws s3 mb " + TEST_BUCKET + " --endpoint " + _endpoint);
    BOOST_CHECK_EQUAL(0, createResult.status);

    // act
    Core::ExecResult result = Core::TestUtils::SendCliCommand("sqs", "aws s3 rb " + TEST_BUCKET + " --endpoint " + _endpoint);
    Database::Entity::S3::BucketList bucketList = _database.ListBuckets();

    // assert
    BOOST_CHECK_EQUAL(0, result.status);
    BOOST_CHECK_EQUAL(0, bucketList.size());
  }*/

}// namespace AwsMock::Service
