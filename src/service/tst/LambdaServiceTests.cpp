//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_SERVICE_LAMBDASERVICETEST_H
#define AWMOCK_SERVICE_LAMBDASERVICETEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include "awsmock/service/lambda/LambdaService.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/repository/LambdaDatabase.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define RUNTIME "java11"
#define FUNCTION_NAME "test-function"
#define ROLE "lambda-role"
#define HANDLER "de.jensvogt.test-lambda.handler"
#define QUALIFIER "latest"
#define ZIP_FILE "/tmp/java-basic-1.0-SNAPSHOT.jar"

namespace AwsMock::Service {

    class LambdaServiceTest : public ::testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
            _database.DeleteAllLambdas();
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

        void CreateTestFunctionAndWaitForActive() {
            Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();

            Dto::Lambda::GetFunctionResponse response = _service.GetFunction(REGION, FUNCTION_NAME);
            while (response.configuration.state != Database::Entity::Lambda::LambdaStateToString(Database::Entity::Lambda::LambdaState::Active)) {
                std::this_thread::sleep_for(500ms);
                response = _service.GetFunction(REGION, FUNCTION_NAME);
            }
        }

        void WaitForActive(const std::string &region, const std::string &functionName) {

            Dto::Lambda::GetFunctionResponse response = _service.GetFunction(region, functionName);
            while (response.configuration.state != Database::Entity::Lambda::LambdaStateToString(Database::Entity::Lambda::LambdaState::Active)) {
                std::this_thread::sleep_for(500ms);
                response = _service.GetFunction(region, functionName);
            }
        }

        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::LambdaDatabase &_database = Database::LambdaDatabase::instance();
        LambdaService _service;
        std::string testFile;
    };

    TEST_F(LambdaServiceTest, LambdaCreateTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();

        // act
        Dto::Lambda::CreateFunctionResponse response = _service.CreateFunction(request);
        long functionCount = _database.LambdaCount();

        // assert
        EXPECT_TRUE(response.handler == HANDLER);
        EXPECT_TRUE(response.runtime == RUNTIME);
        EXPECT_EQ(1, functionCount);
    }

    TEST_F(LambdaServiceTest, LambdaListTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = {{.region = REGION}, FUNCTION_NAME, RUNTIME, ROLE, HANDLER};
        Dto::Lambda::CreateFunctionResponse createResponse = _service.CreateFunction(createRequest);

        // act
        Dto::Lambda::ListFunctionResponse response = _service.ListFunctions(REGION);

        // assert
        EXPECT_FALSE(response.lambdaList.empty());
        EXPECT_TRUE(response.lambdaList.front().function == FUNCTION_NAME);
    }

    TEST_F(LambdaServiceTest, LambdaGetTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest request = CreateTestLambdaRequest();
        Dto::Lambda::CreateFunctionResponse createFunctionResponse = _service.CreateFunction(request);

        // act
        WaitForActive(REGION, FUNCTION_NAME);
        Dto::Lambda::GetFunctionResponse response = _service.GetFunction(REGION, FUNCTION_NAME);

        // assert
        EXPECT_TRUE(response.configuration.handler == HANDLER);
        EXPECT_TRUE(response.configuration.runtime == RUNTIME);
    }

    TEST_F(LambdaServiceTest, LambdaCreateTagsTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = {{.region = REGION}, FUNCTION_NAME, RUNTIME, ROLE, HANDLER};
        Dto::Lambda::CreateFunctionResponse createResponse = _service.CreateFunction(createRequest);
        std::string functionArn = createResponse.functionArn;

        // act
        std::map<std::string, std::string> tags;
        tags["test-key1"] = "test-value1";
        tags["test-key2"] = "test-value2";
        Dto::Lambda::CreateTagRequest createTagRequest = {.arn = functionArn, .tags = tags};
        _service.CreateTag(createTagRequest);
        Database::Entity::Lambda::Lambda lambda = _database.GetLambdaByArn(functionArn);

        // assert
        EXPECT_FALSE(lambda.tags.empty());
        EXPECT_TRUE(lambda.tags["test-key1"] == "test-value1");
        EXPECT_TRUE(lambda.tags["test-key2"] == "test-value2");
    }

    TEST_F(LambdaServiceTest, LambdaDeleteTagsTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = {{.region = REGION}, FUNCTION_NAME, RUNTIME, ROLE, HANDLER};
        Dto::Lambda::CreateFunctionResponse createResponse = _service.CreateFunction(createRequest);
        std::string functionArn = createResponse.functionArn;
        std::map<std::string, std::string> tags;
        tags["test-key1"] = "test-value1";
        tags["test-key2"] = "test-value2";
        Dto::Lambda::CreateTagRequest createTagRequest = {.arn = functionArn, .tags = tags};
        _service.CreateTag(createTagRequest);

        // act
        std::vector<std::string> tagKeys = {"test-key1", "test-key2"};
        Dto::Lambda::DeleteTagsRequest deleteTabsRequest(functionArn, tagKeys);
        _service.DeleteTags(deleteTabsRequest);
        Database::Entity::Lambda::Lambda lambda = _database.GetLambdaByArn(functionArn);

        // assert
        EXPECT_TRUE(lambda.tags.empty());
    }

    TEST_F(LambdaServiceTest, LambdaDeleteTest) {

        // arrange
        Dto::Lambda::CreateFunctionRequest createRequest = {{.region = REGION}, FUNCTION_NAME, RUNTIME, ROLE, HANDLER};
        Dto::Lambda::CreateFunctionResponse createResponse = _service.CreateFunction(createRequest);

        // act
        Dto::Lambda::DeleteFunctionRequest deleteRequest = {.functionName = FUNCTION_NAME, .qualifier = QUALIFIER};
        _service.DeleteFunction(deleteRequest);
        long functionCount = _database.LambdaCount();

        // assert
        EXPECT_EQ(0, functionCount);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_LAMBDASERVICETEST_H