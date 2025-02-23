//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_SERVICE_LAMBDASERVICETEST_H
#define AWMOCK_SERVICE_LAMBDASERVICETEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/lambda/LambdaService.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define RUNTIME "java11"
#define FUNCTION_NAME "test-function"
#define ROLE "lambda-role"
#define HANDLER "de.jensvogt.test-lambda.handler"
#define QUALIFIER "latest"
#define ZIP_FILE "data/lambda/java-basic-1.0-SNAPSHOT.jar"

namespace AwsMock::Service {

    class LambdaServiceTest : public ::testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
            try {
                Dto::Lambda::DeleteFunctionRequest deleteFunctionRequest = {.functionName = FUNCTION_NAME, .qualifier = "latest"};
                _lambdaService.DeleteFunction({.region = REGION, .functionName = FUNCTION_NAME, .qualifier = "latest"});
                _s3Database.DeleteBucket({.region = REGION, .name = "lambda"});
            } catch (Core::ServiceException &ex) {
                // Do nothing
            }
        }

        static Dto::Lambda::CreateFunctionRequest CreateTestLambdaRequest() {
            Dto::Lambda::Code code;
            code.zipFile = ZIP_FILE;
            Dto::Lambda::CreateFunctionRequest request = {{.region = REGION}, FUNCTION_NAME, RUNTIME, ROLE, HANDLER};
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

        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::LambdaDatabase &_database = Database::LambdaDatabase::instance();
        Database::S3Database &_s3Database = Database::S3Database::instance();
        LambdaService _lambdaService;
        std::string testFile;
    };

    TEST_F(LambdaServiceTest, LambdaCreateTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();

        // act
        const Dto::Lambda::CreateFunctionResponse response = _lambdaService.CreateFunction(request);
        WaitForActive(REGION, FUNCTION_NAME);
        const long functionCount = _database.LambdaCount();

        // assert
        EXPECT_TRUE(response.handler == HANDLER);
        EXPECT_TRUE(response.runtime == RUNTIME);
        EXPECT_EQ(1, functionCount);
    }

    TEST_F(LambdaServiceTest, LambdaListTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = CreateTestLambdaRequest();
        Dto::Lambda::CreateFunctionResponse createResponse = _lambdaService.CreateFunction(createRequest);
        WaitForActive(REGION, FUNCTION_NAME);

        // act
        auto [lambdaList, functions] = _lambdaService.ListFunctions(REGION);

        // assert
        EXPECT_FALSE(lambdaList.empty());
        EXPECT_TRUE(lambdaList.front().function == FUNCTION_NAME);
    }

    TEST_F(LambdaServiceTest, LambdaGetTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();
        Dto::Lambda::CreateFunctionResponse createFunctionResponse = _lambdaService.CreateFunction(request);
        WaitForActive(REGION, FUNCTION_NAME);

        // act
        const Dto::Lambda::GetFunctionResponse response = _lambdaService.GetFunction(REGION, FUNCTION_NAME);

        // assert
        EXPECT_TRUE(response.configuration.handler == HANDLER);
        EXPECT_TRUE(response.configuration.runtime == RUNTIME);
    }

    TEST_F(LambdaServiceTest, LambdaCreateTagsTest) {

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
        EXPECT_FALSE(lambda.tags.empty());
        EXPECT_TRUE(lambda.tags["test-key1"] == "test-value1");
        EXPECT_TRUE(lambda.tags["test-key2"] == "test-value2");
    }

    TEST_F(LambdaServiceTest, LambdaDeleteTagsTest) {

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
        Dto::Lambda::DeleteTagsRequest deleteTabsRequest(functionArn, tagKeys);
        _lambdaService.DeleteTags(deleteTabsRequest);
        const Database::Entity::Lambda::Lambda lambda = _database.GetLambdaByArn(functionArn);

        // assert
        EXPECT_TRUE(lambda.tags.empty());
    }

    TEST_F(LambdaServiceTest, LambdaDeleteTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = CreateTestLambdaRequest();
        Dto::Lambda::CreateFunctionResponse createResponse = _lambdaService.CreateFunction(createRequest);
        WaitForActive(REGION, FUNCTION_NAME);

        // act
        const Dto::Lambda::DeleteFunctionRequest deleteRequest = {.region = REGION, .functionName = FUNCTION_NAME, .qualifier = QUALIFIER};
        _lambdaService.DeleteFunction(deleteRequest);
        const long functionCount = _database.LambdaCount();

        // assert
        EXPECT_EQ(0, functionCount);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_LAMBDASERVICETEST_H