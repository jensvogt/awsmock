//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/LambdaDatabase.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_LAMBDA_NAME "test-lambda"
#define TEST_RUNTIME "java21"
#define TEST_ROLE "arn:aws:iam::000000000000:role/ignoreme"
#define TEST_HANDLER "org.springframework.cloud.function.adapter.aws.FunctionInvoker"

namespace AwsMock::Database {

    Entity::Lambda::Lambda CreateDefaultLambda(const std::string &region, const std::string &name) {
        Entity::Lambda::Lambda lambda;
        lambda.region = region;
        lambda.function = name;
        lambda.runtime = TEST_RUNTIME;
        lambda.arn = Core::AwsUtils::CreateLambdaArn(TEST_REGION, TEST_ACCOUNT_ID, name);
        lambda.role = TEST_ROLE;
        lambda.handler = TEST_HANDLER;
        return lambda;
    }

    struct LambdaMemoryDbFixture {
        LambdaMemoryDbFixture() = default;
        ~LambdaMemoryDbFixture() {
            const long count = LambdaDatabase::instance().DeleteAllLambdas();
            log_debug << "Lambda deleted, count: " << count;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(LambdaMemoryDbTests, LambdaMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(LambdaCreate) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);

        // act
        const Entity::Lambda::Lambda result = lambdaDatabase.CreateLambda(lambda);

        // assert
        BOOST_CHECK_EQUAL(result.function, TEST_LAMBDA_NAME);
        BOOST_CHECK_EQUAL(result.runtime, TEST_RUNTIME);
        BOOST_CHECK_EQUAL(result.role, TEST_ROLE);
        BOOST_CHECK_EQUAL(result.handler, TEST_HANDLER);
    }


    BOOST_AUTO_TEST_CASE(LambdaCount) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaDatabase.CreateLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const long result = lambdaDatabase.LambdaCount(lambda.region);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(LambdaExists) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaDatabase.CreateLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const bool result = lambdaDatabase.LambdaExists(TEST_REGION, TEST_LAMBDA_NAME, TEST_RUNTIME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(LambdaGetById) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaDatabase.CreateLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const Entity::Lambda::Lambda result = lambdaDatabase.GetLambdaById(lambda.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, lambda.oid);
    }

    BOOST_AUTO_TEST_CASE(LambdaGetByArn) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaDatabase.CreateLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const Entity::Lambda::Lambda result = lambdaDatabase.GetLambdaByArn(lambda.arn);

        // assert
        BOOST_CHECK_EQUAL(result.arn, lambda.arn);
    }

    BOOST_AUTO_TEST_CASE(LambdaUpdate) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaDatabase.CreateLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        lambda.role = "new_role";
        const Entity::Lambda::Lambda result = lambdaDatabase.UpdateLambda(lambda);

        // assert
        BOOST_CHECK_EQUAL(result.role, lambda.role);
    }

    BOOST_AUTO_TEST_CASE(LambdaList) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaDatabase.CreateLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const std::vector<Entity::Lambda::Lambda> result = lambdaDatabase.ListLambdas(lambda.region);

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
        BOOST_CHECK_EQUAL(result[0].runtime, TEST_RUNTIME);
        BOOST_CHECK_EQUAL(result[0].role, TEST_ROLE);
        BOOST_CHECK_EQUAL(result[0].handler, TEST_HANDLER);
    }

    BOOST_AUTO_TEST_CASE(LambdaDelete) {

        // arrange
        const LambdaDatabase &lambdaDatabase = LambdaDatabase::instance();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaDatabase.CreateLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        lambdaDatabase.DeleteLambda(lambda.function);
        const bool result = lambdaDatabase.LambdaExists(TEST_REGION, TEST_LAMBDA_NAME, TEST_RUNTIME);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database