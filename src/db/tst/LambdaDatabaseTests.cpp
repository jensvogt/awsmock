//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_LAMBDA_DATABASE_TEST_H
#define AWMOCK_CORE_LAMBDA_DATABASE_TEST_H

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/LambdaDatabase.h>

#define ARN "arn:aws:lambda:eu-central-1:000000000000:function:ftp-file-copy"
#define FUNCTION "ftp-file-copy"
#define RUNTIME "Java11"
#define ROLE "arn:aws:iam::000000000000:role/ignoreme"
#define HANDLER "org.springframework.cloud.function.adapter.aws.FunctionInvoker"

namespace AwsMock::Database {

    struct LambdaDatabaseTest {

        LambdaDatabaseTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
            _accountId = _configuration.GetValue<std::string>("awsmock.access.account-id");
        }

        ~LambdaDatabaseTest() {
            long count = _lambdaDatabase.DeleteAllLambdas();
            log_debug << "Lambdas deleted, count: " << count;
            count = _lambdaDatabase.DeleteAllResultsCounters();
            log_debug << "Lambda results deleted, count: " << count;
        }

        [[nodiscard]] Entity::Lambda::LambdaResult createDefaultLambdaResult(const std::string &arn) const {
            Entity::Lambda::LambdaResult lambdaResult;
            lambdaResult.region = _region;
            lambdaResult.lambdaName = FUNCTION;
            lambdaResult.lambdaArn = arn;
            lambdaResult.runtime = "Java21";
            lambdaResult.requestBody = "requestBody";
            lambdaResult.responseBody = "responseBody";
            return lambdaResult;
        }

        std::string _region;
        std::string _accountId;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        LambdaDatabase _lambdaDatabase = LambdaDatabase();
    };

    BOOST_FIXTURE_TEST_CASE(LambdaCreateTest, LambdaDatabaseTest) {

        // arrange
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};

        // act
        const Entity::Lambda::Lambda result = _lambdaDatabase.CreateLambda(lambda);

        // assert
        BOOST_CHECK_EQUAL(result.function, FUNCTION);
        BOOST_CHECK_EQUAL(result.runtime, RUNTIME);
        BOOST_CHECK_EQUAL(result.role, ROLE);
        BOOST_CHECK_EQUAL(result.handler, HANDLER);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaCountTest, LambdaDatabaseTest) {

        // arrange
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        const long result = _lambdaDatabase.LambdaCount(lambda.region);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaExistsTest, LambdaDatabaseTest) {

        // arrange
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        const bool result = _lambdaDatabase.LambdaExists(_region, FUNCTION, RUNTIME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaGetByIdTest, LambdaDatabaseTest) {

        // arrange
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        const Entity::Lambda::Lambda result = _lambdaDatabase.GetLambdaById(lambda.oid);

        // assert
        BOOST_CHECK_EQUAL(result.oid, lambda.oid);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaGetByArnTest, LambdaDatabaseTest) {

        // arrange
        const std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        const Entity::Lambda::Lambda result = _lambdaDatabase.GetLambdaByArn(arn);

        // assert
        BOOST_CHECK_EQUAL(result.arn, lambda.arn);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaUpdateTest, LambdaDatabaseTest) {

        // arrange
        const std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        lambda.role = "new_role";
        const Entity::Lambda::Lambda result = _lambdaDatabase.UpdateLambda(lambda);

        // assert
        BOOST_CHECK_EQUAL(result.role, lambda.role);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaListTest, LambdaDatabaseTest) {

        // arrange
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .codeSize = 1000};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        const std::vector<Entity::Lambda::Lambda> result = _lambdaDatabase.ListLambdas(lambda.region);

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
        BOOST_CHECK_EQUAL(result[0].runtime, RUNTIME);
        BOOST_CHECK_EQUAL(result[0].role, ROLE);
        BOOST_CHECK_EQUAL(result[0].handler, HANDLER);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaDeleteTest, LambdaDatabaseTest) {

        // arrange
        const std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);

        // act
        _lambdaDatabase.DeleteLambda(lambda.function);
        const bool result = _lambdaDatabase.LambdaExists(_region, FUNCTION, RUNTIME);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaResultCreateTest, LambdaDatabaseTest) {

        // arrange
        const std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);
        Entity::Lambda::LambdaResult lambdaResult = createDefaultLambdaResult(lambda.arn);
        lambdaResult = _lambdaDatabase.CreateLambdaResult(lambdaResult);

        // act
        const long count = _lambdaDatabase.LambdaResultsCount(lambda.arn);
        const bool result = _lambdaDatabase.LambdaResultExists(lambdaResult.oid);

        // assert
        BOOST_CHECK_EQUAL(result, true);
        BOOST_CHECK_EQUAL(count, 1);
    }

    BOOST_FIXTURE_TEST_CASE(LambdaResultDeleteTest, LambdaDatabaseTest) {

        // arrange
        const std::string arn = Core::AwsUtils::CreateLambdaArn(_region, _accountId, FUNCTION);
        Entity::Lambda::Lambda lambda = {.region = _region, .function = FUNCTION, .runtime = RUNTIME, .role = ROLE, .handler = HANDLER, .arn = arn};
        lambda = _lambdaDatabase.CreateLambda(lambda);
        Entity::Lambda::LambdaResult lambdaResult = createDefaultLambdaResult(lambda.arn);
        lambdaResult = _lambdaDatabase.CreateLambdaResult(lambdaResult);

        // act
        const long count = _lambdaDatabase.DeleteResultsCounter(lambdaResult.oid);
        const bool result = _lambdaDatabase.LambdaResultExists(lambdaResult.oid);

        // assert
        BOOST_CHECK_EQUAL(result, false);
        BOOST_CHECK_EQUAL(count, 1);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_LAMBDA_DATABASE_TEST_H