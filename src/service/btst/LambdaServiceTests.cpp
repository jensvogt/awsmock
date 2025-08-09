//
// Created by vogje01 on 02/06/2023.
//

// AwsMock includes

#include "TestBase.h"


#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/lambda/LambdaService.h>

#define BOOST_TEST_MODULE LambdaServiceTests
#define REGION "eu-central-1"
#define OWNER "test-owner"
#define RUNTIME "java11"
#define FUNCTION_NAME "test-function"
#define ROLE "lambda-role"
#define HANDLER "de.jensvogt.test-lambda.handler"
#define QUALIFIER "latest"
#define ZIP_FILE "data/lambda/java-basic-1.0-SNAPSHOT.jar"

namespace AwsMock::Service {

    struct LambdaServiceTest : TestBase {

        LambdaServiceTest() {
            try {
                Dto::Lambda::DeleteFunctionRequest deleteFunctionRequest = {.functionName = FUNCTION_NAME, .qualifier = "latest"};
                _lambdaService.DeleteFunction({.region = REGION, .functionName = FUNCTION_NAME, .qualifier = "latest"});
                Database::Entity::S3::Bucket bucket;
                bucket.region = REGION;
                bucket.name = "lambda";
                _s3Database.DeleteBucket(bucket);
            } catch (Core::ServiceException &ex) {
                // Do nothing
            }
        }

        static Dto::Lambda::CreateFunctionRequest CreateTestLambdaRequest() {
            Dto::Lambda::Code code;
            code.zipFile = ZIP_FILE;
            Dto::Lambda::CreateFunctionRequest request;
            request.requestId = Core::StringUtils::CreateRandomUuid();
            request.region = REGION;
            request.user = "user";
            request.functionName = FUNCTION_NAME;
            request.runtime = RUNTIME;
            request.role = ROLE;
            request.handler = HANDLER;
            request.ephemeralStorage = Dto::Lambda::EphemeralStorage();
            request.environment = Dto::Lambda::EnvironmentVariables();
            request.code = code;
            return request;
        }

        void WaitForActive(const std::string &region, const std::string &functionName) const {

            Dto::Lambda::GetFunctionResponse response = _lambdaService.GetFunction(region, functionName);
            while (response.configuration.state != LambdaStateToString(Database::Entity::Lambda::LambdaState::Active)) {
                std::this_thread::sleep_for(500ms);
                response = _lambdaService.GetFunction(region, functionName);
            }
        }

        boost::asio::io_context _ioContext;
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::LambdaDatabase &_database = Database::LambdaDatabase::instance();
        Database::S3Database &_s3Database = Database::S3Database::instance();
        LambdaService _lambdaService{_ioContext};
        std::string testFile;
    };

    BOOST_FIXTURE_TEST_CASE(LambdaCreateTest, LambdaServiceTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();

        // act
        const Dto::Lambda::CreateFunctionResponse response = _lambdaService.CreateFunction(request);
        WaitForActive(REGION, FUNCTION_NAME);
        const long functionCount = _database.LambdaCount();

        // assert
        BOOST_CHECK_EQUAL(response.handler, HANDLER);
        BOOST_CHECK_EQUAL(response.runtime, RUNTIME);
        BOOST_CHECK_EQUAL(1, functionCount);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaListTest, LambdaServiceTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = CreateTestLambdaRequest();
        Dto::Lambda::CreateFunctionResponse createResponse = _lambdaService.CreateFunction(createRequest);
        WaitForActive(REGION, FUNCTION_NAME);

        // act
        auto functions = _lambdaService.ListFunctions(REGION);

        // assert
        BOOST_CHECK_EQUAL(functions.functions.empty(), false);
        BOOST_CHECK_EQUAL(functions.functions.front().functionName, FUNCTION_NAME);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaGetTest, LambdaServiceTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();
        Dto::Lambda::CreateFunctionResponse createFunctionResponse = _lambdaService.CreateFunction(request);
        WaitForActive(REGION, FUNCTION_NAME);

        // act
        const Dto::Lambda::GetFunctionResponse response = _lambdaService.GetFunction(REGION, FUNCTION_NAME);

        // assert
        BOOST_CHECK_EQUAL(response.configuration.handler, HANDLER);
        BOOST_CHECK_EQUAL(response.configuration.runtime, RUNTIME);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaCreateTagsTest, LambdaServiceTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();
        const Dto::Lambda::CreateFunctionResponse createFunctionResponse = _lambdaService.CreateFunction(request);
        const std::string functionArn = createFunctionResponse.functionArn;
        WaitForActive(REGION, FUNCTION_NAME);

        // act
        std::map<std::string, std::string> tags;
        tags["test-key1"] = "test-value1";
        tags["test-key2"] = "test-value2";
        const Dto::Lambda::CreateTagRequest createTagRequest = {.arn = functionArn, .tags = tags};
        _lambdaService.CreateTag(createTagRequest);
        Database::Entity::Lambda::Lambda lambda = _database.GetLambdaByArn(functionArn);

        // assert
        BOOST_CHECK_EQUAL(lambda.tags.empty(), false);
        BOOST_CHECK_EQUAL(lambda.tags["test-key1"], "test-value1");
        BOOST_CHECK_EQUAL(lambda.tags["test-key2"], "test-value2");
    }

    BOOST_FIXTURE_TEST_CASE(LambdaDeleteTagsTest, LambdaServiceTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();
        const Dto::Lambda::CreateFunctionResponse createFunctionResponse = _lambdaService.CreateFunction(request);
        WaitForActive(REGION, FUNCTION_NAME);

        const std::string functionArn = createFunctionResponse.functionArn;
        std::map<std::string, std::string> tags;
        tags["test-key1"] = "test-value1";
        tags["test-key2"] = "test-value2";
        const Dto::Lambda::CreateTagRequest createTagRequest = {.arn = functionArn, .tags = tags};
        _lambdaService.CreateTag(createTagRequest);

        // act
        const std::vector<std::string> tagKeys = {"test-key1", "test-key2"};
        const Dto::Lambda::DeleteTagsRequest deleteTabsRequest(functionArn, tagKeys);
        _lambdaService.DeleteTags(deleteTabsRequest);
        const Database::Entity::Lambda::Lambda lambda = _database.GetLambdaByArn(functionArn);

        // assert
        BOOST_CHECK_EQUAL(lambda.tags.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaDeleteTest, LambdaServiceTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = CreateTestLambdaRequest();
        Dto::Lambda::CreateFunctionResponse createResponse = _lambdaService.CreateFunction(createRequest);
        WaitForActive(REGION, FUNCTION_NAME);

        // act
        const Dto::Lambda::DeleteFunctionRequest deleteRequest = {.region = REGION, .functionName = FUNCTION_NAME, .qualifier = QUALIFIER};
        _lambdaService.DeleteFunction(deleteRequest);
        const long functionCount = _database.LambdaCount();

        // assert
        BOOST_CHECK_EQUAL(0, functionCount);
    }

}// namespace AwsMock::Service
