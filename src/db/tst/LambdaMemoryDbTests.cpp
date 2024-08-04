//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_LAMBDAMEMORYDBTEST_H
#define AWMOCK_CORE_LAMBDAMEMORYDBTEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/LambdaDatabase.h>

#define ARN "arn:aws:lambda:eu-central-1:000000000000:function:ftp-file-copy"
#define FUNCTION "ftp-file-copy"
#define RUNTIME "Java11"
#define ROLE "arn:aws:iam::000000000000:role/ignoreme"
#define HANDLER "org.springframework.cloud.function.adapter.aws.FunctionInvoker"

namespace AwsMock::Database {

    class LambdaMemoryDbTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.getString("awsmock.region");
            _accountId = _configuration.getString("awsmock.account.id");
        }

        void TearDown() override {
            _lambdaDatabase.DeleteAllLambdas();
        }

        std::string _region;
        std::string _accountId;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        LambdaDatabase &_lambdaDatabase = LambdaDatabase::instance();
    };

    TEST_F(LambdaMemoryDbTest, LambdaCreateTest) {

        // arrange
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};

        // act
        Entity::Lambda::Lambda result = _lambdaDatabase.CreateLambda(lambda);

        // assert
        EXPECT_TRUE(result.function == FUNCTION);
        EXPECT_TRUE(result.runtime == RUNTIME);
        EXPECT_TRUE(result.role == ROLE);
        EXPECT_TRUE(result.handler == HANDLER);
    }

    TEST_F(LambdaMemoryDbTest, LambdaCountTest) {

        // arrange
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        _lambdaDatabase.CreateLambda(lambda);

        // act
        long result = _lambdaDatabase.LambdaCount(lambda.region);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(LambdaMemoryDbTest, LambdaExistsTest) {

        // arrange
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        _lambdaDatabase.CreateLambda(lambda);

        // act
        bool result = _lambdaDatabase.LambdaExists(_region, FUNCTION, RUNTIME);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(LambdaMemoryDbTest, LambdaGetByIdTest) {

        // arrange
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        Entity::Lambda::Lambda result = _lambdaDatabase.GetLambdaById(lambda.oid);

        // assert
        EXPECT_EQ(result.oid, lambda.oid);
    }

    TEST_F(LambdaMemoryDbTest, LambdaGetByArnTest) {

        // arrange
        std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        Entity::Lambda::Lambda result = _lambdaDatabase.GetLambdaByArn(arn);

        // assert
        EXPECT_EQ(result.arn, lambda.arn);
    }

    TEST_F(LambdaMemoryDbTest, LambdaUpdateTest) {

        // arrange
        std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        lambda.role = "new_role";
        Entity::Lambda::Lambda result = _lambdaDatabase.UpdateLambda(lambda);

        // assert
        EXPECT_EQ(result.role, lambda.role);
    }

    TEST_F(LambdaMemoryDbTest, LambdaListTest) {

        // arrange
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        std::vector<Entity::Lambda::Lambda> result = _lambdaDatabase.ListLambdas(lambda.region);

        // assert
        EXPECT_EQ(1, result.size());
        EXPECT_TRUE(result[0].runtime == RUNTIME);
        EXPECT_TRUE(result[0].role == ROLE);
        EXPECT_TRUE(result[0].handler == HANDLER);
    }

    TEST_F(LambdaMemoryDbTest, LambdaDeleteTest) {

        // arrange
        std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda
                lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        _lambdaDatabase.DeleteLambda(lambda.function);
        bool result = _lambdaDatabase.LambdaExists(_region, FUNCTION, RUNTIME);

        // assert
        EXPECT_FALSE(result);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_LAMBDAMEMORYDBTEST_H