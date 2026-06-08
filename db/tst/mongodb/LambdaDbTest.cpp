//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/RepositoryFactory.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_LAMBDA_NAME "test-lambda"
#define TEST_RUNTIME "java21"
#define TEST_ROLE "arn:aws:iam::000000000000:role/ignoreme"
#define TEST_HANDLER "org.springframework.cloud.function.adapter.aws.FunctionInvoker"

namespace Awsmock::Database {

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
        LambdaMemoryDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB, "test");
        }
        ~LambdaMemoryDbFixture() {
            const long count = RepositoryFactory::instance().lambdaRepository()->deleteAllLambdas();
            log_debug << "Lambda deleted, count: " << count;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(LambdaMemoryDbTests, LambdaMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(LambdaCreate) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);

        // act
        const Entity::Lambda::Lambda result = lambdaRepository->createLambda(lambda);

        // assert
        BOOST_CHECK_EQUAL(result.function, TEST_LAMBDA_NAME);
        BOOST_CHECK_EQUAL(result.runtime, TEST_RUNTIME);
        BOOST_CHECK_EQUAL(result.role, TEST_ROLE);
        BOOST_CHECK_EQUAL(result.handler, TEST_HANDLER);
    }


    BOOST_AUTO_TEST_CASE(lambdaCount) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaRepository->createLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const long result = lambdaRepository->lambdaCount(lambda.region);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(lambdaExists) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaRepository->createLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const bool result = lambdaRepository->lambdaExists(TEST_REGION, TEST_LAMBDA_NAME, TEST_RUNTIME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(LambdaGetById) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaRepository->createLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const Entity::Lambda::Lambda result = lambdaRepository->getLambdaById(lambda.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, lambda.oid);
    }

    BOOST_AUTO_TEST_CASE(LambdaGetByArn) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaRepository->createLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const Entity::Lambda::Lambda result = lambdaRepository->getLambdaByArn(lambda.arn);

        // assert
        BOOST_CHECK_EQUAL(result.arn, lambda.arn);
    }

    BOOST_AUTO_TEST_CASE(LambdaUpdate) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaRepository->createLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        lambda.role = "new_role";
        const Entity::Lambda::Lambda result = lambdaRepository->updateLambda(lambda);

        // assert
        BOOST_CHECK_EQUAL(result.role, lambda.role);
    }

    BOOST_AUTO_TEST_CASE(LambdaList) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaRepository->createLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        const std::vector<Entity::Lambda::Lambda> result = lambdaRepository->listLambdas(lambda.region);

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
        BOOST_CHECK_EQUAL(result[0].runtime, TEST_RUNTIME);
        BOOST_CHECK_EQUAL(result[0].role, TEST_ROLE);
        BOOST_CHECK_EQUAL(result[0].handler, TEST_HANDLER);
    }

    BOOST_AUTO_TEST_CASE(LambdaDelete) {

        // arrange
        const std::shared_ptr<ILambdaRepository> lambdaRepository = RepositoryFactory::instance().lambdaRepository();
        Entity::Lambda::Lambda lambda = CreateDefaultLambda(TEST_REGION, TEST_LAMBDA_NAME);
        lambda = lambdaRepository->createLambda(lambda);
        BOOST_CHECK_EQUAL(lambda.function, TEST_LAMBDA_NAME);

        // act
        lambdaRepository->deleteLambda(lambda.function);
        const bool result = lambdaRepository->lambdaExists(TEST_REGION, TEST_LAMBDA_NAME, TEST_RUNTIME);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database