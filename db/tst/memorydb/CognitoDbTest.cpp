//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/CognitoDatabase.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_USER_POOL_NAME "test-user_pool"

namespace AwsMock::Database {

    Entity::Cognito::UserPool CreateDefaultUserPool(const std::string &region, const std::string &name) {
        Entity::Cognito::UserPool userPool;
        userPool.region = region;
        userPool.name = name;
        return userPool;
    }

    BOOST_AUTO_TEST_CASE(CreateUserpoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userpool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);

        // act
        userpool = cognitoDatabase.CreateUserPool(userpool);

        // assert
        BOOST_CHECK_EQUAL(false, userpool.arn.empty());
        BOOST_CHECK_EQUAL(false, userpool.oid.empty());
        BOOST_CHECK_EQUAL(false, userpool.name.empty());
    }

}// namespace AwsMock::Database